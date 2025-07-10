/*
 * Smart Beer Tap System - Main Controller
 *
 * This is the main Arduino sketch for the automated beer dispensing system.
 * It coordinates between the LED controller, pour system, and network manager
 * to provide a complete beer tap automation solution.
 *
 * Key Features:
 * - Precise volume control with flow sensor
 * - Visual feedback through LED patterns
 * - Remote control via ThingsBoard IoT platform
 * - Comprehensive safety systems
 * - WiFi connectivity with auto-reconnection
 */

// Include configuration and modules
#include "src/config.h"
#include "src/constants.h"
#include <WiFi.h>
#include <Arduino_MQTT_Client.h>
#include <Server_Side_RPC.h>
#include <ThingsBoard.h>
#include "src/led_controller.h"
#include "src/pour_system.h"
#include "src/network_manager.h"
#include "src/config_validator.h"
#include "src/status_manager.h"

// Initialize ThingsBoard client
WiFiClient espClient;
Arduino_MQTT_Client mqttClient(espClient);
constexpr size_t MAX_RPC_SUBSCRIPTIONS = 3U;
constexpr size_t MAX_RPC_RESPONSE = 256U;
Server_Side_RPC<MAX_RPC_SUBSCRIPTIONS, MAX_RPC_RESPONSE> rpc;
IAPI_Implementation *apis[1U] = {
    &rpc};
ThingsBoard tb(mqttClient, 256U, 256U, 1024U, apis + 0U, apis + 1U);

// Connection state tracking
bool thingsBoardConnected = false;
bool rpcSubscribed = false;
unsigned long lastConnectionAttempt = 0;
const unsigned long CONNECTION_RETRY_INTERVAL = 5000; // 5 seconds
const unsigned long CONNECTION_TIMEOUT = 10000;       // 10 seconds timeout for connection attempt

// Helper macro for array size
#define COUNT_OF(x) ((sizeof(x) / sizeof(0 [x])) / ((size_t)(!(sizeof(x) % sizeof(0 [x])))))

// Forward declarations
void processCupSizeChange(const JsonVariantConst &data, JsonDocument &response);
void processMlPerPulseChange(const JsonVariantConst &data, JsonDocument &response);
void processStopCommand(const JsonVariantConst &data, JsonDocument &response);

// RPC callback array
const RPC_Callback callbacks[] = {
    {TB_SET_CUP_SIZE_RPC, processCupSizeChange},
    {TB_SET_ML_PER_PULSE_RPC, processMlPerPulseChange},
    {TB_STOP_POUR_RPC, processStopCommand}};

void setup()
{
  Serial.begin(115200);
  Serial.println();
  Serial.println("🍺 Smart Beer Tap System Starting...");
  Serial.println("=====================================");

  // Initialize LED controller first for visual feedback
  ledController.init();
  ledController.blinkCount(1, "System starting up");

  // Validate configuration before proceeding
  if (!configValidator.validateConfiguration())
  {
    Serial.println("");
    Serial.println("❌ SETUP FAILED - Configuration Invalid!");
    configValidator.displayConfigErrors();

    // Set error LED pattern and halt
    ledController.setPattern(LED_CONFIG_ERROR);

    // Don't proceed with initialization
    return;
  }

  // Configuration is valid, proceed with setup
  Serial.println("");
  Serial.println("🚀 Starting hardware initialization...");

  // Initialize pour system
  pourSystem.init();

  // Initialize network connectivity and connect to WiFi
  networkManager.init();

  // Connect to WiFi first
  Serial.println("📡 Connecting to WiFi...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  // Wait for WiFi connection with timeout
  int wifiTimeout = 30; // 30 seconds
  while (WiFi.status() != WL_CONNECTED && wifiTimeout > 0)
  {
    delay(1000);
    Serial.print(".");
    wifiTimeout--;
  }

  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println();
    Serial.println("❌ Failed to connect to WiFi!");
    ledController.setPattern(LED_ERROR_NETWORK);
    return;
  }

  Serial.println();
  Serial.println("✅ WiFi connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // ThingsBoard connection will be handled in loop()
  Serial.println("📡 WiFi ready, ThingsBoard connection will be attempted in main loop...");

  // Set system ready after successful initialization
  statusManager.setReady();

  // System ready indication
  Serial.println("");
  Serial.println("✅ SETUP COMPLETE!");
  Serial.println("🍺 Smart Beer Tap ready for operation");
  Serial.println("📱 Use your ThingsBoard dashboard to control the tap");
  Serial.println("");

  ledController.blinkCount(4, "System ready for operation");

  // Set ready as background pattern
  ledController.setPattern(LED_READY, true);
}

void loop()
{
  // If configuration is invalid, just update LED error pattern and wait
  if (!configValidator.isConfigValid())
  {
    ledController.update();
    delay(100);
    return;
  }

  // Handle ThingsBoard connection
  if (WiFi.status() == WL_CONNECTED)
  {
    if (!thingsBoardConnected)
    {
      // Try to connect to ThingsBoard
      if (millis() - lastConnectionAttempt > CONNECTION_RETRY_INTERVAL)
      {
        Serial.println("📡 Attempting to connect to ThingsBoard...");
        Serial.print("Server: ");
        Serial.println(THINGSBOARD_SERVER);
        Serial.print("Token: ");
        Serial.println(String(THINGSBOARD_ACCESS_TOKEN).substring(0, 8) + "...");

        // Set connection timeout
        unsigned long connectionStart = millis();
        bool connectionResult = false;

        // Try connection with timeout
        while (millis() - connectionStart < CONNECTION_TIMEOUT)
        {
          if (tb.connect(THINGSBOARD_SERVER, THINGSBOARD_ACCESS_TOKEN))
          {
            connectionResult = true;
            break;
          }
          delay(100);
          // Feed watchdog during connection attempt
          pourSystem.checkWatchdog();
        }

        if (connectionResult)
        {
          Serial.println("✅ ThingsBoard connected!");
          thingsBoardConnected = true;

          // Subscribe to RPC commands
          if (rpc.RPC_Subscribe(callbacks + 0U, callbacks + COUNT_OF(callbacks)))
          {
            Serial.println("✅ RPC subscriptions successful!");
            rpcSubscribed = true;

            // Send initial attributes
            tb.sendAttributeData("cupSize", 0);
            tb.sendAttributeData("status", STATUS_READY);
            tb.sendAttributeData("ready", 1);
            tb.sendAttributeData("mlPerPulse", pourSystem.getMlPerPulse());
          }
          else
          {
            Serial.println("❌ RPC subscription failed!");
          }
        }
        else
        {
          Serial.println("❌ ThingsBoard connection failed, retrying...");
          Serial.print("WiFi Status: ");
          Serial.println(WiFi.status());
          Serial.print("IP Address: ");
          Serial.println(WiFi.localIP());
        }
        lastConnectionAttempt = millis();
      }
    }
    else
    {
      // Already connected, run normal loop
      tb.loop();
    }
  }
  else
  {
    // WiFi disconnected, reset ThingsBoard connection
    thingsBoardConnected = false;
    rpcSubscribed = false;
  }

  // Check system watchdog
  pourSystem.checkWatchdog();

  // Update LED patterns non-blocking
  ledController.update();

  // Check network status and handle reconnections
  networkManager.handleWifiStatusChange();
  networkManager.handleThingsBoardStatusChange(pourSystem.getCurrentCupSize(), pourSystem.getMlPerPulse());

  // Update pour system (includes safety checks and pour logic)
  pourSystem.update();

  // Send status updates to ThingsBoard if connected and status changed
  if (thingsBoardConnected)
  {
    static String lastTbStatus = "";
    String currentStatus = pourSystem.getLastStatus();
    if (currentStatus != lastTbStatus && currentStatus.length() > 0)
    {
      tb.sendAttributeData("status", currentStatus);
      Serial.println("📱 Status attribute sent to ThingsBoard: " + currentStatus);

      // Reset cup size display when pour completes
      if (currentStatus == STATUS_COMPLETE)
      {
        tb.sendAttributeData("cupSize", 0);
        Serial.println("📱 Cup size reset to 0 after pour completion");
      }

      lastTbStatus = currentStatus;
    }

    // Send ready/busy status updates to ThingsBoard if changed
    static int lastReadyBusyStatus = -1;
    int currentReadyBusyStatus = statusManager.isReady() ? 1 : 0;
    if (currentReadyBusyStatus != lastReadyBusyStatus)
    {
      tb.sendAttributeData("ready", currentReadyBusyStatus);
      Serial.println("📱 Ready/Busy attribute sent to ThingsBoard: " + String(currentReadyBusyStatus == 1 ? "READY" : "BUSY"));
      lastReadyBusyStatus = currentReadyBusyStatus;
    }
  }

  // Small delay to prevent overwhelming the system
  delay(100);
}

// ThingsBoard RPC callback handlers

void processCupSizeChange(const JsonVariantConst &data, JsonDocument &response)
{
  int value = data.as<int>();
  Serial.print("📱 ThingsBoard received cup size: ");
  Serial.print(value);
  Serial.println("ml");
  pourSystem.handleCupSizeChange(value);

  // Send attribute update to ThingsBoard
  if (thingsBoardConnected)
  {
    tb.sendAttributeData("cupSize", value);
    Serial.println("📱 Cup size attribute sent to ThingsBoard: " + String(value) + "ml");
  }

  response.set(value);
}

void processMlPerPulseChange(const JsonVariantConst &data, JsonDocument &response)
{
  float value = data.as<float>();
  pourSystem.handleMlPerPulseChange(value);

  // Send attribute update to ThingsBoard
  if (thingsBoardConnected)
  {
    tb.sendAttributeData("mlPerPulse", value);
    Serial.println("📱 ML per pulse attribute sent to ThingsBoard: " + String(value));
  }

  response.set(value);
}

void processStopCommand(const JsonVariantConst &data, JsonDocument &response)
{
  int value = data.as<int>();
  if (value == 1) // Button pressed (only act on press, not release)
  {
    pourSystem.emergencyStop();
    // Reset cup size display on dashboard
    if (thingsBoardConnected)
    {
      tb.sendAttributeData("cupSize", 0);
      Serial.println("📱 Dashboard cup size reset to 0");
    }
  }
  response.set("stopped");
}
