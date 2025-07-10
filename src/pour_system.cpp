#include "pour_system.h"

// ThingsBoard RPC functions will be called from main file

// Global instance
PourSystem pourSystem;

// Static member definitions
portMUX_TYPE PourSystem::spinlock = portMUX_INITIALIZER_UNLOCKED;

PourSystem::PourSystem()
{
  pulseCount = 0;
  mlPerPulse = DEFAULT_ML_PER_PULSE;
  totalVolume = 0;
  pourStartTime = 0;
  isPouring = false;
  currentCupSize = 0;
  lastWatchdogTime = 0;
}

void PourSystem::init()
{
  pinMode(FLOW_SENSOR_PIN, INPUT_PULLUP);
  pinMode(RELAY_PIN, OUTPUT);
  setRelay(true); // Ensure relay starts in safe state (closed)

  attachInterrupt(digitalPinToInterrupt(FLOW_SENSOR_PIN), pulseCounter, RISING);

  lastWatchdogTime = millis();
}

void PourSystem::setRelay(bool state)
{
  digitalWrite(RELAY_PIN, state);
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
  isPouring = true;
  setRelay(false);
  pourStartTime = millis();
  Serial.println("Pour started");
}

void PourSystem::stopPour()
{
  setRelay(true);
  Serial.println("Pour complete - " + String(totalVolume) + "ml poured");
  resetCounters();
  isPouring = false;
  currentCupSize = 0; // Reset cup size
  // ThingsBoard attribute update will be called from main file
}

void PourSystem::emergencyStop()
{
  if (isPouring)
  {
    setRelay(true); // Close valve immediately
    Serial.println("🛑 EMERGENCY STOP - Pour halted by user");

    // Reset system state
    resetCounters();
    isPouring = false;
    currentCupSize = 0; // Reset cup size
  }
  else
  {
    Serial.println("ℹ️ Stop button pressed, but no pour in progress");
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
    resetCounters();
    setRelay(true);
    isPouring = false;
  }
  else
  {
    // Input validation
    if (value < MIN_CUP_SIZE || value > MAX_CUP_SIZE)
    {
      Serial.println("❌ Invalid cup size: " + String(value) + "ml");
      return;
    }
    currentCupSize = value;
    isPouring = false; // Reset pouring state
    resetCounters();   // Reset counters for new pour
    Serial.println("✅ Cup size set to " + String(currentCupSize) + "ml");
  }
}

void PourSystem::handleMlPerPulseChange(float value)
{
  // Input validation
  if (value < MIN_ML_PER_PULSE || value > MAX_ML_PER_PULSE)
  {
    Serial.println("❌ Invalid ml per pulse: " + String(value));
    return;
  }
  mlPerPulse = value;
  Serial.println("✅ ML per pulse updated: " + String(mlPerPulse));
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

bool PourSystem::performSafetyChecks(bool wifiConnected, bool thingsBoardConnected)
{
  // Get current pulse count safely
  portENTER_CRITICAL_ISR(&spinlock);
  unsigned long currentPulseCount = pulseCount;
  portEXIT_CRITICAL_ISR(&spinlock);

  // Bounds checking for calculations
  if (currentPulseCount > MAX_PULSE_COUNT)
  {
    Serial.println("Error: Pulse count overflow detected");
    stopPour();
    return false;
  }

  totalVolume = currentPulseCount * mlPerPulse;

  // Check for calculation overflow
  if (totalVolume > MAX_VOLUME_SANITY)
  {
    Serial.println("Error: Volume calculation overflow");
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
      return false;
    }

    // Check for maximum volume
    if (totalVolume > MAX_POUR_VOLUME)
    {
      Serial.println("Maximum pour volume reached!");
      stopPour();
      return false;
    }
  }

  return true;
}


void PourSystem::update()
{
  // Perform safety checks first
  bool wifiConnected = (WiFi.status() == WL_CONNECTED);
  bool thingsBoardConnected = true; // Will be set by main file

  if (!performSafetyChecks(wifiConnected, thingsBoardConnected))
  {
    return; // Safety check failed, exit early
  }


  // Enhanced pour start logic with error handling
  if (isPouring == false && totalVolume == 0 && currentCupSize > 0)
  {
    // Additional safety checks before starting pour
    if (currentCupSize <= 0)
    {
      Serial.println("Error: Invalid cup size for pour start");
      return;
    }

    if (!wifiConnected || !thingsBoardConnected)
    {
      Serial.println("Warning: Starting pour without network connection");
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

}
