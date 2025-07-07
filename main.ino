// Include configuration file with credentials
#include "config.h"

// Blynk virtual pins
#define CUP_SIZE_PIN V1
#define STATUS_PIN V2
#define ML_PER_PULSE_PIN V3

// Hardware pins
#define LED_PIN 2
#define RELAY_PIN 13
#define FLOW_SENSOR_PIN 27

// Flow sensor constants
#define DEFAULT_ML_PER_PULSE 2.222       // 450 pulses/liter ≈ 2.222ml/pulse
float mlPerPulse = DEFAULT_ML_PER_PULSE; // Dynamic value that can be changed via Blynk
static portMUX_TYPE spinlock = portMUX_INITIALIZER_UNLOCKED;

// Safety and monitoring constants
#define MAX_POUR_TIME 90000    // Maximum pour time in milliseconds (90 seconds)
#define MAX_POUR_VOLUME 2000   // Maximum pour volume in ml (2 liters)
#define MIN_CUP_SIZE 50        // Minimum cup size in ml
#define MAX_CUP_SIZE 2000      // Maximum cup size in ml
#define MIN_ML_PER_PULSE 0.5   // Minimum ml per pulse
#define MAX_ML_PER_PULSE 10.0  // Maximum ml per pulse
#define WATCHDOG_TIMEOUT 30000 // Watchdog timeout in milliseconds (30 seconds)

// Status messages
#define STATUS_READY "System Ready"
#define STATUS_POURING "Pouring..."
#define STATUS_COMPLETE "Pour Complete!"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

// Variables
// Pour tracking variables
volatile unsigned long pulseCount = 0; // Flow sensor pulse counter
float totalVolume = 0;                 // Current pour volume in ml
unsigned long pourStartTime = 0;       // Time when pour started
unsigned long totalPours = 0;          // Total number of pours
float totalVolumePoured = 0;           // Total volume poured in ml
unsigned long lastWatchdogTime = 0;    // Last watchdog timer reset

// LED Status System
enum LedPattern
{
  LED_OFF,
  LED_SOLID_ON,
  LED_READY,           // Slow single blink every 2 seconds
  LED_POURING,         // Fast continuous toggle
  LED_POUR_COMPLETE,   // 3 medium blinks
  LED_SYSTEM_START,    // 1 long blink
  LED_SYSTEM_READY,    // 4 quick blinks
  LED_CUP_SIZE_CHANGE, // 2 medium blinks
  LED_WIFI_CONNECT,    // 2 quick blinks
  LED_BLYNK_CONNECT,   // 2 quick blinks with pause
  LED_ERROR_CRITICAL,  // Very fast continuous blink
  LED_ERROR_WARNING,   // Medium speed blink
  LED_ERROR_INPUT,     // Short-long-short pattern
  LED_ERROR_TIMEOUT,   // Long-short-long pattern
  LED_ERROR_VOLUME,    // Short-short-long pattern
  LED_ERROR_NETWORK,   // Long-long-short pattern
  LED_ERROR_SENSOR     // Short-long-long pattern
};

struct LedState
{
  LedPattern currentPattern;
  LedPattern backgroundPattern;
  unsigned long lastUpdate;
  unsigned long patternStartTime;
  int stepIndex;
  bool ledPhysicalState;
  bool patternActive;
};

LedState ledController = {LED_OFF, LED_OFF, 0, 0, 0, false, false};

// System state flags
bool isReady = true;             // System ready for new pour
bool isPouring = false;          // Pour in progress
bool lastWifiConnected = false;  // Track last WiFi connection state
bool lastBlynkConnected = false; // Track last Blynk connection state

// Configuration
int currentCupSize = 0; // Target pour volume in ml
String lastStatus = ""; // Last status message sent to Blynk

// Helper functions
void setLed(bool state)
{
  digitalWrite(LED_PIN, state);
  ledController.ledPhysicalState = state;
}

void setLedPattern(LedPattern pattern, bool isBackground = false)
{
  if (isBackground)
  {
    ledController.backgroundPattern = pattern;
  }
  else
  {
    ledController.currentPattern = pattern;
    ledController.patternStartTime = millis();
    ledController.stepIndex = 0;
    ledController.patternActive = true;
  }
}

void updateLedController()
{
  unsigned long currentTime = millis();
  LedPattern activePattern = ledController.currentPattern != LED_OFF ? ledController.currentPattern : ledController.backgroundPattern;

  if (activePattern == LED_OFF)
  {
    setLed(false);
    return;
  }

  // Pattern timing definitions
  switch (activePattern)
  {
  case LED_SOLID_ON:
    setLed(true);
    break;

  case LED_READY:
    if (currentTime - ledController.lastUpdate > 2000)
    {
      setLed(!ledController.ledPhysicalState);
      ledController.lastUpdate = currentTime;
    }
    break;

  case LED_POURING:
    if (currentTime - ledController.lastUpdate > 250)
    {
      setLed(!ledController.ledPhysicalState);
      ledController.lastUpdate = currentTime;
    }
    break;

  case LED_POUR_COMPLETE:
  {
    int intervals[] = {200, 200, 200, 200, 200, 200, 1000}; // 3 blinks + pause
    if (ledController.stepIndex < 7)
    {
      if (currentTime - ledController.lastUpdate > intervals[ledController.stepIndex])
      {
        setLed(ledController.stepIndex % 2 == 0);
        ledController.lastUpdate = currentTime;
        ledController.stepIndex++;
      }
    }
    else
    {
      ledController.currentPattern = LED_OFF;
    }
  }
  break;

  case LED_SYSTEM_START:
    if (currentTime - ledController.patternStartTime < 500)
    {
      setLed(true);
    }
    else if (currentTime - ledController.patternStartTime < 1000)
    {
      setLed(false);
    }
    else
    {
      ledController.currentPattern = LED_OFF;
    }
    break;

  case LED_SYSTEM_READY:
  {
    int intervals[] = {150, 150, 150, 150, 150, 150, 150, 150, 1000}; // 4 quick blinks
    if (ledController.stepIndex < 9)
    {
      if (currentTime - ledController.lastUpdate > intervals[ledController.stepIndex])
      {
        setLed(ledController.stepIndex % 2 == 0);
        ledController.lastUpdate = currentTime;
        ledController.stepIndex++;
      }
    }
    else
    {
      ledController.currentPattern = LED_OFF;
    }
  }
  break;

  case LED_CUP_SIZE_CHANGE:
  {
    int intervals[] = {200, 200, 200, 200, 1000}; // 2 medium blinks
    if (ledController.stepIndex < 5)
    {
      if (currentTime - ledController.lastUpdate > intervals[ledController.stepIndex])
      {
        setLed(ledController.stepIndex % 2 == 0);
        ledController.lastUpdate = currentTime;
        ledController.stepIndex++;
      }
    }
    else
    {
      ledController.currentPattern = LED_OFF;
    }
  }
  break;

  case LED_WIFI_CONNECT:
  {
    int intervals[] = {150, 150, 150, 150, 1000}; // 2 quick blinks
    if (ledController.stepIndex < 5)
    {
      if (currentTime - ledController.lastUpdate > intervals[ledController.stepIndex])
      {
        setLed(ledController.stepIndex % 2 == 0);
        ledController.lastUpdate = currentTime;
        ledController.stepIndex++;
      }
    }
    else
    {
      ledController.currentPattern = LED_OFF;
    }
  }
  break;

  case LED_BLYNK_CONNECT:
  {
    int intervals[] = {150, 150, 300, 150, 150, 1000}; // 2 quick blinks with pause
    if (ledController.stepIndex < 6)
    {
      if (currentTime - ledController.lastUpdate > intervals[ledController.stepIndex])
      {
        setLed(ledController.stepIndex % 2 == 0 && ledController.stepIndex != 2);
        ledController.lastUpdate = currentTime;
        ledController.stepIndex++;
      }
    }
    else
    {
      ledController.currentPattern = LED_OFF;
    }
  }
  break;

  case LED_ERROR_CRITICAL:
    if (currentTime - ledController.lastUpdate > 100)
    {
      setLed(!ledController.ledPhysicalState);
      ledController.lastUpdate = currentTime;
    }
    break;

  case LED_ERROR_WARNING:
    if (currentTime - ledController.lastUpdate > 300)
    {
      setLed(!ledController.ledPhysicalState);
      ledController.lastUpdate = currentTime;
    }
    break;

  case LED_ERROR_INPUT:
  {
    int intervals[] = {100, 100, 300, 100, 100, 100, 1000}; // Short-long-short + pause
    if (ledController.stepIndex < 7)
    {
      if (currentTime - ledController.lastUpdate > intervals[ledController.stepIndex])
      {
        setLed(ledController.stepIndex % 2 == 0);
        ledController.lastUpdate = currentTime;
        ledController.stepIndex++;
      }
    }
    else
    {
      ledController.stepIndex = 0;
      ledController.lastUpdate = currentTime;
    }
  }
  break;

  case LED_ERROR_TIMEOUT:
  {
    int intervals[] = {300, 100, 100, 100, 300, 100, 1000}; // Long-short-long + pause
    if (ledController.stepIndex < 7)
    {
      if (currentTime - ledController.lastUpdate > intervals[ledController.stepIndex])
      {
        setLed(ledController.stepIndex % 2 == 0);
        ledController.lastUpdate = currentTime;
        ledController.stepIndex++;
      }
    }
    else
    {
      ledController.stepIndex = 0;
      ledController.lastUpdate = currentTime;
    }
  }
  break;

  case LED_ERROR_VOLUME:
  {
    int intervals[] = {100, 100, 100, 100, 300, 100, 1000}; // Short-short-long + pause
    if (ledController.stepIndex < 7)
    {
      if (currentTime - ledController.lastUpdate > intervals[ledController.stepIndex])
      {
        setLed(ledController.stepIndex % 2 == 0);
        ledController.lastUpdate = currentTime;
        ledController.stepIndex++;
      }
    }
    else
    {
      ledController.stepIndex = 0;
      ledController.lastUpdate = currentTime;
    }
  }
  break;

  case LED_ERROR_NETWORK:
  {
    int intervals[] = {300, 100, 300, 100, 100, 100, 1000}; // Long-long-short + pause
    if (ledController.stepIndex < 7)
    {
      if (currentTime - ledController.lastUpdate > intervals[ledController.stepIndex])
      {
        setLed(ledController.stepIndex % 2 == 0);
        ledController.lastUpdate = currentTime;
        ledController.stepIndex++;
      }
    }
    else
    {
      ledController.stepIndex = 0;
      ledController.lastUpdate = currentTime;
    }
  }
  break;

  case LED_ERROR_SENSOR:
  {
    int intervals[] = {100, 100, 300, 100, 300, 100, 1000}; // Short-long-long + pause
    if (ledController.stepIndex < 7)
    {
      if (currentTime - ledController.lastUpdate > intervals[ledController.stepIndex])
      {
        setLed(ledController.stepIndex % 2 == 0);
        ledController.lastUpdate = currentTime;
        ledController.stepIndex++;
      }
    }
    else
    {
      ledController.stepIndex = 0;
      ledController.lastUpdate = currentTime;
    }
  }
  break;

  default:
    setLed(false);
    break;
  }
}

void setRelay(bool state)
{
  digitalWrite(RELAY_PIN, state);
}

void updateStatus(const String &status)
{
  if (status != lastStatus)
  {
    Blynk.virtualWrite(STATUS_PIN, status);
    lastStatus = status;
  }
}

void resetCounters()
{
  portENTER_CRITICAL_ISR(&spinlock);
  pulseCount = 0;
  portEXIT_CRITICAL_ISR(&spinlock);
  totalVolume = 0;
}

void startPour()
{
  // Bounds checking for totalPours
  if (totalPours >= ULONG_MAX - 1)
  {
    Serial.println("Warning: Pour count approaching maximum");
    totalPours = 0; // Reset to prevent overflow
  }

  isPouring = true;
  setRelay(false);
  setLedPattern(LED_POURING, true); // Set as background pattern
  updateStatus(STATUS_POURING);
  pourStartTime = millis();
  totalPours++;
}

void stopPour()
{
  setRelay(true);

  // Bounds checking for totalVolumePoured
  if (totalVolumePoured > (ULONG_MAX - totalVolume - 1000))
  {
    Serial.println("Warning: Total volume approaching maximum, resetting");
    totalVolumePoured = 0; // Reset to prevent overflow
  }

  totalVolumePoured += totalVolume;
  blinkCount(3, "Pour complete - " + String(totalVolume) + "ml poured");
  setLedPattern(LED_READY, true); // Set ready as background pattern
  updateStatus(STATUS_COMPLETE);
  resetCounters();
  isPouring = false;
  isReady = true;
  Blynk.virtualWrite(CUP_SIZE_PIN, 0);
}

void IRAM_ATTR pulseCounter()
{
  portENTER_CRITICAL_ISR(&spinlock);
  pulseCount++;
  portEXIT_CRITICAL_ISR(&spinlock);
  // LED toggling removed from ISR to prevent race conditions
}

// Legacy LED functions - replaced with pattern system
void blinkRapidly(int duration, String reason)
{
  Serial.println("Status: " + reason);
  // Use appropriate error pattern based on reason
  if (reason.indexOf("timeout") >= 0)
  {
    setLedPattern(LED_ERROR_TIMEOUT);
  }
  else if (reason.indexOf("volume") >= 0)
  {
    setLedPattern(LED_ERROR_VOLUME);
  }
  else if (reason.indexOf("WiFi") >= 0 || reason.indexOf("Blynk") >= 0)
  {
    setLedPattern(LED_ERROR_NETWORK);
  }
  else if (reason.indexOf("Invalid") >= 0)
  {
    setLedPattern(LED_ERROR_INPUT);
  }
  else if (reason.indexOf("sensor") >= 0)
  {
    setLedPattern(LED_ERROR_SENSOR);
  }
  else
  {
    setLedPattern(LED_ERROR_CRITICAL);
  }
}

void blinkCount(int count, String reason)
{
  Serial.println("Status: " + reason);
  // Use appropriate pattern based on count and reason
  if (count == 1)
  {
    setLedPattern(LED_SYSTEM_START);
  }
  else if (count == 2)
  {
    if (reason.indexOf("WiFi") >= 0)
    {
      setLedPattern(LED_WIFI_CONNECT);
    }
    else if (reason.indexOf("Blynk") >= 0)
    {
      setLedPattern(LED_BLYNK_CONNECT);
    }
    else
    {
      setLedPattern(LED_CUP_SIZE_CHANGE);
    }
  }
  else if (count == 3)
  {
    setLedPattern(LED_POUR_COMPLETE);
  }
  else if (count == 4)
  {
    setLedPattern(LED_SYSTEM_READY);
  }
}

// Blynk handlers

BLYNK_WRITE(CUP_SIZE_PIN)
{
  int value = param.asInt();
  if (value == 0)
  {
    isReady = true;
    resetCounters();
    setRelay(true);
    setLed(false);
    isPouring = false;
  }
  else
  {
    // Input validation
    if (value < MIN_CUP_SIZE || value > MAX_CUP_SIZE)
    {
      updateStatus("Error: Invalid cup size " + String(value) + "ml");
      blinkRapidly(1000, "Invalid cup size");
      return;
    }
    currentCupSize = value;
    isReady = false;
    isPouring = false; // Reset pouring state
    resetCounters();   // Reset counters for new pour
    blinkCount(2, "Cup size changed to " + String(currentCupSize) + "ml");
  }
}

BLYNK_WRITE(ML_PER_PULSE_PIN)
{
  float value = param.asFloat();
  // Input validation
  if (value < MIN_ML_PER_PULSE || value > MAX_ML_PER_PULSE)
  {
    updateStatus("Error: Invalid ml/pulse " + String(value));
    blinkRapidly(1000, "Invalid ml per pulse");
    return;
  }
  mlPerPulse = value;
  updateStatus("ML per pulse updated: " + String(mlPerPulse));
}

void setup()
{
  Serial.begin(115200);
  blinkCount(1, "System starting up");
  lastWatchdogTime = millis(); // Initialize watchdog timer

  pinMode(FLOW_SENSOR_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);
  setRelay(true); // Ensure relay starts in safe state (closed)

  attachInterrupt(digitalPinToInterrupt(FLOW_SENSOR_PIN), pulseCounter, RISING);

  // Show connecting pattern
  setLedPattern(LED_ERROR_WARNING); // Use warning pattern for connecting
  Serial.println("Status: Connecting to WiFi");
  Blynk.begin(BLYNK_AUTH_TOKEN, WIFI_SSID, WIFI_PASSWORD);

  // Initialize Blynk values to safe defaults
  Blynk.virtualWrite(CUP_SIZE_PIN, 0);
  Blynk.virtualWrite(ML_PER_PULSE_PIN, mlPerPulse);
  updateStatus(STATUS_READY);
  blinkCount(4, "System ready for operation");

  // Set ready as background pattern
  setLedPattern(LED_READY, true);
}

void checkWatchdog()
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

void loop()
{
  Blynk.run();
  checkWatchdog();
  updateLedController(); // Update LED patterns non-blocking

  // Check WiFi connection status with recovery
  bool wifiConnected = (WiFi.status() == WL_CONNECTED);
  if (wifiConnected != lastWifiConnected)
  {
    if (wifiConnected)
    {
      blinkCount(2, "WiFi connection established");
    }
    else
    {
      blinkRapidly(1000, "WiFi connection failed");
      WiFi.reconnect();
    }
    lastWifiConnected = wifiConnected;
  }

  // Check Blynk connection status with recovery
  bool blynkConnected = Blynk.connected();
  if (blynkConnected != lastBlynkConnected)
  {
    if (blynkConnected)
    {
      blinkCount(2, "Blynk connection established");
      // Resync Blynk values after reconnection
      Blynk.virtualWrite(CUP_SIZE_PIN, currentCupSize);
      Blynk.virtualWrite(ML_PER_PULSE_PIN, mlPerPulse);
    }
    else
    {
      blinkRapidly(1000, "Blynk connection failed");
    }
    lastBlynkConnected = blynkConnected;
  }
  portENTER_CRITICAL_ISR(&spinlock);
  unsigned long currentPulseCount = pulseCount;
  portEXIT_CRITICAL_ISR(&spinlock);

  // Bounds checking for calculations
  if (currentPulseCount > 1000000) // Sanity check for pulse count
  {
    Serial.println("Error: Pulse count overflow detected");
    updateStatus("Error: Sensor malfunction");
    stopPour();
    return;
  }

  totalVolume = currentPulseCount * mlPerPulse;

  // Check for calculation overflow
  if (totalVolume > 10000) // 10L sanity check
  {
    Serial.println("Error: Volume calculation overflow");
    updateStatus("Error: Volume overflow");
    stopPour();
    return;
  }

  // Safety checks
  if (isPouring)
  {
    // Check for timeout (skip during continuous pour mode)
    if (millis() - pourStartTime > MAX_POUR_TIME)
    {
      Serial.println("Pour timeout reached!");
      stopPour();
      updateStatus("Error: Pour timeout");
      blinkRapidly(2000, "Pour timeout error");
    }

    // Check for maximum volume (redundant safety check)
    if (totalVolume > MAX_POUR_VOLUME)
    {
      Serial.println("Maximum pour volume reached!");
      stopPour();
      updateStatus("Error: Max volume reached");
      blinkRapidly(2000, "Max volume error");
    }

    // Redundant safety checks

    // Check for sensor disconnection (no pulses for extended time)
    static unsigned long lastPulseTime = 0;
    if (isPouring && pulseCount > 0)
    {
      lastPulseTime = millis();
    }
    else if (isPouring && (millis() - lastPulseTime > 10000)) // 10 seconds without pulses
    {
      Serial.println("Warning: No flow detected - sensor may be disconnected");
      updateStatus("Warning: No flow detected");
    }
  }

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

  if (isPouring && totalVolume >= currentCupSize)
  {
    Serial.print(currentCupSize);
    Serial.println("ml reached! Stopping pour...");
    stopPour();
  }

  // Print system stats every 5 seconds
  static unsigned long lastStatsTime = 0;
  if (millis() - lastStatsTime > 5000)
  {
    Serial.print("Total pours: ");
    Serial.print(totalPours);
    Serial.print(", Total volume: ");
    Serial.print(totalVolumePoured);
    Serial.println("ml");
    lastStatsTime = millis();
  }

  // LED patterns are now handled by updateLedController()

  delay(100);
}