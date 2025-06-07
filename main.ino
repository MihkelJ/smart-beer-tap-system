// Blynk credentials
#define BLYNK_TEMPLATE_ID ""
#define BLYNK_TEMPLATE_NAME ""
#define BLYNK_AUTH_TOKEN ""

// Blynk virtual pins
#define CUP_SIZE_PIN V1
#define STATUS_PIN V2
#define ML_PER_PULSE_PIN V3
#define CONTINUOUS_POUR_PIN V4

// Hardware pins
#define LED_PIN 2
#define RELAY_PIN 13
#define FLOW_SENSOR_PIN 27

// Flow sensor constants
#define DEFAULT_ML_PER_PULSE 2.222       // 450 pulses/liter ≈ 2.222ml/pulse
float mlPerPulse = DEFAULT_ML_PER_PULSE; // Dynamic value that can be changed via Blynk
static portMUX_TYPE spinlock = portMUX_INITIALIZER_UNLOCKED;

// Safety and monitoring constants
#define MAX_POUR_TIME 90000  // Maximum pour time in milliseconds (90 seconds)
#define MAX_POUR_VOLUME 2000 // Maximum pour volume in ml (2 liters)

// Status messages
#define STATUS_READY "System Ready"
#define STATUS_POURING "Pouring..."
#define STATUS_COMPLETE "Pour Complete!"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

// WiFi credentials
const char *ssid = "";
const char *password = "";

// Variables
// Pour tracking variables
volatile unsigned long pulseCount = 0; // Flow sensor pulse counter
float totalVolume = 0;                 // Current pour volume in ml
unsigned long pourStartTime = 0;       // Time when pour started
unsigned long totalPours = 0;          // Total number of pours
float totalVolumePoured = 0;           // Total volume poured in ml
unsigned long systemStartTime = 0;     // System start time for uptime tracking

// System state flags
bool isReady = true;             // System ready for new pour
bool isPouring = false;          // Pour in progress
bool lastWifiConnected = false;  // Track last WiFi connection state
bool lastBlynkConnected = false; // Track last Blynk connection state
bool continuousPour = false;     // Continuous pouring mode flag

// Configuration
int currentCupSize = 0; // Target pour volume in ml
String lastStatus = ""; // Last status message sent to Blynk

// Helper functions
void setLed(bool state)
{
  digitalWrite(LED_PIN, state);
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
  isPouring = true;
  setRelay(false);
  setLed(true);
  updateStatus(STATUS_POURING);
  pourStartTime = millis();
  totalPours++;
}

void stopPour()
{
  setRelay(true);
  totalVolumePoured += totalVolume;
  blinkCount(3, "Pour complete - " + String(totalVolume) + "ml poured");
  setLed(false);
  updateStatus(STATUS_COMPLETE);
  resetCounters();
  isPouring = false;
  isReady = true;
  Blynk.virtualWrite(CUP_SIZE_PIN, 0);
}

void IRAM_ATTR pulseCounter()
{
  pulseCount++;
  setLed(!digitalRead(LED_PIN));
}

// LED pattern functions
void blinkRapidly(int duration, String reason)
{
  Serial.println("Status: " + reason);
  unsigned long startTime = millis();
  while (millis() - startTime < duration)
  {
    setLed(true);
    delay(100);
    setLed(false);
    delay(100);
  }
  delay(500);
}

void blinkCount(int count, String reason)
{
  Serial.println("Status: " + reason);
  for (int i = 0; i < count; i++)
  {
    setLed(true);
    delay(200);
    setLed(false);
    delay(200);
  }
  delay(500);
}

// Blynk handlers
BLYNK_WRITE(CONTINUOUS_POUR_PIN)
{
  continuousPour = param.asInt();
  if (continuousPour)
  {
    startPour();
    updateStatus("Continuous Pour Mode Active");
    blinkCount(5, "Continuous pour mode activated");
  }
  else
  {
    stopPour();
  }
}

BLYNK_WRITE(CUP_SIZE_PIN)
{
  int value = param.asInt();
  if (value == 0 && !continuousPour)
  {
    isReady = true;
    resetCounters();
    setRelay(true);
    setLed(false);
    isPouring = false;
  }
  else if (!continuousPour)
  {
    currentCupSize = value;
    isReady = false;
    isPouring = false; // Reset pouring state
    resetCounters();   // Reset counters for new pour
    blinkCount(2, "Cup size changed to " + String(currentCupSize) + "ml");
  }
}

BLYNK_WRITE(ML_PER_PULSE_PIN)
{
  mlPerPulse = param.asFloat();
  updateStatus("ML per pulse updated: " + String(mlPerPulse));
}

void setup()
{
  Serial.begin(115200);
  blinkCount(1, "System starting up");
  systemStartTime = millis();

  pinMode(FLOW_SENSOR_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);
  setRelay(true);

  attachInterrupt(digitalPinToInterrupt(FLOW_SENSOR_PIN), pulseCounter, RISING);

  blinkRapidly(2000, "Connecting to WiFi");
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, password);

  Blynk.virtualWrite(CUP_SIZE_PIN, 0);
  Blynk.virtualWrite(ML_PER_PULSE_PIN, mlPerPulse);
  Blynk.virtualWrite(CONTINUOUS_POUR_PIN, 0);
  updateStatus(STATUS_READY);
  blinkCount(4, "System ready for operation");
}

void loop()
{
  Blynk.run();

  // Check WiFi connection status
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

  // Check Blynk connection status
  bool blynkConnected = Blynk.connected();
  if (blynkConnected != lastBlynkConnected)
  {
    if (blynkConnected)
    {
      blinkCount(2, "Blynk connection established");
    }
    else
    {
      blinkRapidly(1000, "Blynk connection failed");
    }
    lastBlynkConnected = blynkConnected;
  }
  portENTER_CRITICAL_ISR(&spinlock);  
  totalVolume = pulseCount * mlPerPulse;
  portEXIT_CRITICAL_ISR(&spinlock); 

  // Safety checks
  if (isPouring)
  {
    // Check for timeout
    if (millis() - pourStartTime > MAX_POUR_TIME)
    {
      Serial.println("Pour timeout reached!");
      stopPour();
      updateStatus("Error: Pour timeout");
      blinkRapidly(2000, "Pour timeout error");
    }

    // Check for maximum volume
    if (totalVolume > MAX_POUR_VOLUME)
    {
      Serial.println("Maximum pour volume reached!");
      stopPour();
      updateStatus("Error: Max volume reached");
      blinkRapidly(2000, "Max volume error");
    }
  }

  if (!isReady && !isPouring && totalVolume == 0 && !continuousPour)
  {
    startPour();
  }

  if (isPouring && totalVolume >= currentCupSize && !continuousPour)
  {
    Serial.print(currentCupSize);
    Serial.println("ml reached! Stopping pour...");
    stopPour();
  }

  // Print system stats every 5 seconds
  static unsigned long lastStatsTime = 0;
  if (millis() - lastStatsTime > 5000)
  {
    Serial.print("Uptime: ");
    Serial.print((millis() - systemStartTime) / 1000);
    Serial.print(", Total pours: ");
    Serial.print(totalPours);
    Serial.print(", Total volume: ");
    Serial.print(totalVolumePoured);
    Serial.println("ml");
    lastStatsTime = millis();
  }

  delay(100);
}
