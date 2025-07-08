#include "pour_system.h"

// Blynk functions will be called from main file

// Global instance
PourSystem pourSystem;

// Static member definitions
portMUX_TYPE PourSystem::spinlock = portMUX_INITIALIZER_UNLOCKED;
unsigned long PourSystem::lastPulseTime = 0;

PourSystem::PourSystem()
{
  pulseCount = 0;
  mlPerPulse = DEFAULT_ML_PER_PULSE;
  totalVolume = 0;
  pourStartTime = 0;
  totalPours = 0;
  totalVolumePoured = 0;
  isReady = true;
  isPouring = false;
  currentCupSize = 0;
  lastStatus = "";
  lastWatchdogTime = 0;
  lastStatsTime = 0;
}

void PourSystem::init()
{
  pinMode(FLOW_SENSOR_PIN, INPUT_PULLUP);
  pinMode(RELAY_PIN, OUTPUT);
  setRelay(true); // Ensure relay starts in safe state (closed)
  
  attachInterrupt(digitalPinToInterrupt(FLOW_SENSOR_PIN), pulseCounter, RISING);
  
  lastWatchdogTime = millis();
  lastStatsTime = millis();
  
  // Initialize status manager
  statusManager.init();
}

void PourSystem::setRelay(bool state)
{
  digitalWrite(RELAY_PIN, state);
}

void PourSystem::updateStatus(const String &status)
{
  if (status != lastStatus)
  {
    lastStatus = status;
    Serial.println("Status: " + status);
    // Note: Blynk.virtualWrite() must be called from main file due to module separation
  }
}

void PourSystem::resetCounters()
{
  portENTER_CRITICAL_ISR(&spinlock);
  pulseCount = 0;
  portEXIT_CRITICAL_ISR(&spinlock);
  totalVolume = 0;
}

void PourSystem::startPour()
{
  // Bounds checking for totalPours
  if (totalPours >= ULONG_MAX - 1)
  {
    Serial.println("Warning: Pour count approaching maximum");
    totalPours = 0; // Reset to prevent overflow
  }

  isPouring = true;
  statusManager.setBusy();
  setRelay(false);
  ledController.setPattern(LED_POURING, true); // Set as background pattern
  updateStatus(STATUS_POURING);
  pourStartTime = millis();
  totalPours++;
}

void PourSystem::stopPour()
{
  setRelay(true);

  // Bounds checking for totalVolumePoured
  if (totalVolumePoured > (ULONG_MAX - totalVolume - 1000))
  {
    Serial.println("Warning: Total volume approaching maximum, resetting");
    totalVolumePoured = 0; // Reset to prevent overflow
  }

  totalVolumePoured += totalVolume;
  ledController.blinkCount(3, "Pour complete - " + String(totalVolume) + "ml poured");
  ledController.setPattern(LED_READY, true); // Set ready as background pattern
  updateStatus(STATUS_COMPLETE);
  resetCounters();
  isPouring = false;
  isReady = true;
  statusManager.setReady();
  // Blynk.virtualWrite(CUP_SIZE_PIN, 0) will be called from main file
}

void PourSystem::emergencyStop()
{
  if (isPouring)
  {
    setRelay(true);  // Close valve immediately
    ledController.blinkRapidly(1000, "Emergency stop activated");
    updateStatus("❌ Pour stopped by user");
    Serial.println("🛑 EMERGENCY STOP - Pour halted by user");
    
    // Reset system state
    resetCounters();
    isPouring = false;
    isReady = true;
    currentCupSize = 0;  // Reset cup size
    statusManager.setReady();
    
    // Set ready pattern after brief error indication
    ledController.setPattern(LED_READY, true);
  }
  else
  {
    Serial.println("ℹ️ Stop button pressed, but no pour in progress");
    updateStatus("ℹ️ No pour to stop");
  }
}

void IRAM_ATTR PourSystem::pulseCounter()
{
  portENTER_CRITICAL_ISR(&spinlock);
  pourSystem.pulseCount++;
  portEXIT_CRITICAL_ISR(&spinlock);
  // LED toggling removed from ISR to prevent race conditions
}

void PourSystem::handleCupSizeChange(int value)
{
  if (value == 0)
  {
    isReady = true;
    resetCounters();
    setRelay(true);
    ledController.setLed(false);
    isPouring = false;
    statusManager.setReady();
  }
  else
  {
    // Input validation
    if (value < MIN_CUP_SIZE || value > MAX_CUP_SIZE)
    {
      updateStatus("Error: Invalid cup size " + String(value) + "ml");
      ledController.blinkRapidly(1000, "Invalid cup size");
      statusManager.setBusy();
      return;
    }
    currentCupSize = value;
    isReady = false;
    isPouring = false; // Reset pouring state
    resetCounters();   // Reset counters for new pour
    ledController.blinkCount(2, "Cup size changed to " + String(currentCupSize) + "ml");
    statusManager.setBusy();
  }
}

void PourSystem::handleMlPerPulseChange(float value)
{
  // Input validation
  if (value < MIN_ML_PER_PULSE || value > MAX_ML_PER_PULSE)
  {
    updateStatus("Error: Invalid ml/pulse " + String(value));
    ledController.blinkRapidly(1000, "Invalid ml per pulse");
    return;
  }
  mlPerPulse = value;
  updateStatus("ML per pulse updated: " + String(mlPerPulse));
}

void PourSystem::checkWatchdog()
{
  // Simple software watchdog - reset if system becomes unresponsive
  if (millis() - lastWatchdogTime > WATCHDOG_TIMEOUT)
  {
    Serial.println("Watchdog timeout - forcing system reset");
    stopPour();    // Emergency stop
    ESP.restart(); // Restart the system
  }
  lastWatchdogTime = millis();
}

bool PourSystem::performSafetyChecks(bool wifiConnected, bool blynkConnected)
{
  // Get current pulse count safely
  portENTER_CRITICAL_ISR(&spinlock);
  unsigned long currentPulseCount = pulseCount;
  portEXIT_CRITICAL_ISR(&spinlock);

  // Bounds checking for calculations
  if (currentPulseCount > MAX_PULSE_COUNT)
  {
    Serial.println("Error: Pulse count overflow detected");
    updateStatus("Error: Sensor malfunction");
    stopPour();
    return false;
  }

  totalVolume = currentPulseCount * mlPerPulse;

  // Check for calculation overflow
  if (totalVolume > MAX_VOLUME_SANITY)
  {
    Serial.println("Error: Volume calculation overflow");
    updateStatus("Error: Volume overflow");
    stopPour();
    return false;
  }

  // Safety checks during pouring
  if (isPouring)
  {
    // Check for timeout
    if (millis() - pourStartTime > MAX_POUR_TIME)
    {
      Serial.println("Pour timeout reached!");
      stopPour();
      updateStatus("Error: Pour timeout");
      ledController.blinkRapidly(2000, "Pour timeout error");
      return false;
    }

    // Check for maximum volume
    if (totalVolume > MAX_POUR_VOLUME)
    {
      Serial.println("Maximum pour volume reached!");
      stopPour();
      updateStatus("Error: Max volume reached");
      ledController.blinkRapidly(2000, "Max volume error");
      return false;
    }
  }

  return true;
}

void PourSystem::checkSensorHealth()
{
  // Sensor health check disabled - uncomment if you have flow sensor connected
  /*
  // Check for sensor disconnection (no pulses for extended time)
  if (isPouring && pulseCount > 0)
  {
    lastPulseTime = millis();
  }
  else if (isPouring && (millis() - lastPulseTime > SENSOR_TIMEOUT))
  {
    Serial.println("Warning: No flow detected - sensor may be disconnected");
    updateStatus("Warning: No flow detected");
  }
  */
}

void PourSystem::update()
{
  // Perform safety checks first
  bool wifiConnected = (WiFi.status() == WL_CONNECTED);
  bool blynkConnected = true; // Will be set by main file
  
  if (!performSafetyChecks(wifiConnected, blynkConnected))
  {
    return; // Safety check failed, exit early
  }
  
  // Sensor health checks removed

  // Enhanced pour start logic with error handling
  if (!isReady && !isPouring && totalVolume == 0)
  {
    // Additional safety checks before starting pour
    if (currentCupSize <= 0)
    {
      Serial.println("Error: Invalid cup size for pour start");
      updateStatus("Error: Invalid cup size");
      isReady = true; // Reset to ready state
      return;
    }

    if (!wifiConnected || !blynkConnected)
    {
      Serial.println("Warning: Starting pour without network connection");
      updateStatus("Warning: No network connection");
    }

    startPour();
  }

  // Check if target volume reached
  if (isPouring && totalVolume >= currentCupSize)
  {
    Serial.print(currentCupSize);
    Serial.println("ml reached! Stopping pour...");
    stopPour();
  }

  // Print system stats every 5 seconds
  if (millis() - lastStatsTime > 5000)
  {
    Serial.print("Total pours: ");
    Serial.print(totalPours);
    Serial.print(", Total volume: ");
    Serial.print(totalVolumePoured);
    Serial.println("ml");
    lastStatsTime = millis();
  }
}