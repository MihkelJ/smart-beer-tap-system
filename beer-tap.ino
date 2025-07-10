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
#include <Arduino_MQTT_Client.h>
#include <Server_Side_RPC.h>
#include <ThingsBoard.h>
#include <WiFi.h>
#include <WiFiManager.h>  // WiFiManager by Tzapu - Install via Arduino Library Manager
#include "src/config.h"
#include "src/config_validator.h"
#include "src/constants.h"
#include "src/led_controller.h"
#include "src/network_manager.h"
#include "src/pour_system.h"

// Initialize ThingsBoard client
WiFiClient espClient;
Arduino_MQTT_Client mqttClient(espClient);
constexpr size_t MAX_RPC_SUBSCRIPTIONS = 4U;
constexpr size_t MAX_RPC_RESPONSE = 256U;
Server_Side_RPC<MAX_RPC_SUBSCRIPTIONS, MAX_RPC_RESPONSE> rpc;
IAPI_Implementation *apis[1U] = {&rpc};
ThingsBoard tb(mqttClient, 256U, 256U, 1024U, apis + 0U, apis + 1U);

// LED controller instance
LEDController ledController;

// WiFi Manager instance for dynamic WiFi configuration
WiFiManager wifiManager;

// Custom WiFiManager parameters
WiFiManagerParameter *custom_tb_server = nullptr;

// Connection state tracking
bool thingsBoardConnected = false;
bool rpcSubscribed = false;
unsigned long lastConnectionAttempt = 0;
const unsigned long CONNECTION_RETRY_INTERVAL = 5000;  // 5 seconds
const unsigned long CONNECTION_TIMEOUT = 10000;        // 10 seconds timeout for connection attempt

// Helper macro for array size
#define COUNT_OF(x) ((sizeof(x) / sizeof(0 [x])) / ((size_t)(!(sizeof(x) % sizeof(0 [x])))))

// Forward declarations
void processCupSizeChange(const JsonVariantConst &data, JsonDocument &response);
void processMlPerPulseChange(const JsonVariantConst &data, JsonDocument &response);
void processStopCommand(const JsonVariantConst &data, JsonDocument &response);
void processWiFiResetCommand(const JsonVariantConst &data, JsonDocument &response);

// RPC callback array
const RPC_Callback callbacks[] = {{TB_SET_CUP_SIZE_RPC, processCupSizeChange},
                                  {TB_SET_ML_PER_PULSE_RPC, processMlPerPulseChange},
                                  {TB_STOP_POUR_RPC, processStopCommand},
                                  {TB_RESET_WIFI_RPC, processWiFiResetCommand}};

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("🍺 Smart Beer Tap System Starting...");
  Serial.println("=====================================");

  // Initialize LED controller first for visual feedback
  ledController.begin();
  ledController.setState(STATE_BOOTING);

  // Validate configuration before proceeding
  if (!configValidator.validateConfiguration()) {
    Serial.println("");
    Serial.println("❌ SETUP FAILED - Configuration Invalid!");
    configValidator.displayConfigErrors();

    // Indicate configuration error with LED
    ledController.setState(STATE_CONFIG_ERROR);

    // Don't proceed with initialization
    return;
  }

  // Configuration is valid, proceed with setup
  Serial.println("");
  Serial.println("🚀 Starting hardware initialization...");

  // Initialize pour system
  pourSystem.init();

  // Initialize network connectivity
  networkManager.init();

  // Configure WiFi Manager
  Serial.println("📡 Starting WiFi configuration...");
  ledController.setState(STATE_WIFI_PORTAL_ACTIVE);

  // Configure WiFiManager settings
  wifiManager.setTimeout(WIFI_PORTAL_TIMEOUT);
  wifiManager.setConfigPortalBlocking(false);
  wifiManager.setDebugOutput(true);

  // Custom branding and information
  wifiManager.setTitle("🍺 Beer Tap WiFi Setup");
  wifiManager.setDarkMode(true);

  // Optional: Add custom parameters for ThingsBoard server override
  custom_tb_server = new WiFiManagerParameter("tb_server", "ThingsBoard Server (optional)",
                                              THINGSBOARD_SERVER, 50);
  wifiManager.addParameter(custom_tb_server);

  // Set custom callback for when user is configuring
  wifiManager.setAPCallback([](WiFiManager *wm) {
    ledController.setState(STATE_WIFI_PORTAL_CONFIG);
    Serial.println("🌐 WiFi provisioning portal started");
    Serial.print("Connect to AP: ");
    Serial.println(WIFI_PORTAL_SSID);
    Serial.println("Browse to: 192.168.4.1");
  });

  // Set callback for save config
  wifiManager.setSaveConfigCallback([]() {
    ledController.setState(STATE_WIFI_CONNECTING);
    Serial.println("📝 WiFi configuration saved, connecting...");

    // Handle custom ThingsBoard server if provided
    if (custom_tb_server != nullptr) {
      String customTbServer = custom_tb_server->getValue();
      if (customTbServer.length() > 0 && customTbServer != THINGSBOARD_SERVER) {
        Serial.print("📝 Custom ThingsBoard server configured: ");
        Serial.println(customTbServer);
        // Note: In production, you might want to save this to SPIFFS/LittleFS
      }
    }
  });

  // Start WiFi Manager - will connect to saved network or start portal
  if (wifiManager.autoConnect(WIFI_PORTAL_SSID, WIFI_PORTAL_PASSWORD)) {
    Serial.println("✅ WiFi connected!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    ledController.setState(STATE_WIFI_CONNECTED);
  } else {
    Serial.println("❌ WiFi connection failed or timed out");
    ledController.setState(STATE_WIFI_FAILED);
    return;
  }

  // ThingsBoard connection will be handled in loop()
  Serial.println("📡 WiFi ready, ThingsBoard connection will be attempted in main loop...");

  // System ready indication
  Serial.println("");
  Serial.println("✅ SETUP COMPLETE!");
  Serial.println("🍺 Smart Beer Tap ready for operation");
  Serial.println("📱 Use your ThingsBoard dashboard to control the tap");
  Serial.println("");

  // Set system ready LED status
  ledController.setState(STATE_SYSTEM_READY);
}

void loop() {
  // Update LED controller
  ledController.update();

  // Process WiFi Manager events (non-blocking)
  wifiManager.process();

  // If configuration is invalid, just wait
  if (!configValidator.isConfigValid()) {
    ledController.setState(STATE_CONFIG_ERROR);
    delay(100);
    return;
  }

  // Handle ThingsBoard connection
  if (WiFi.status() == WL_CONNECTED) {
    if (!thingsBoardConnected) {
      // Try to connect to ThingsBoard
      if (millis() - lastConnectionAttempt > CONNECTION_RETRY_INTERVAL) {
        Serial.println("📡 Attempting to connect to ThingsBoard...");
        Serial.print("Server: ");
        Serial.println(THINGSBOARD_SERVER);
        Serial.print("Token: ");
        Serial.println(String(THINGSBOARD_ACCESS_TOKEN).substring(0, 8) + "...");

        // Set ThingsBoard connecting LED status
        ledController.setState(STATE_TB_CONNECTING);

        // Set connection timeout
        unsigned long connectionStart = millis();
        bool connectionResult = false;

        // Try connection with timeout
        while (millis() - connectionStart < CONNECTION_TIMEOUT) {
          if (tb.connect(THINGSBOARD_SERVER, THINGSBOARD_ACCESS_TOKEN)) {
            connectionResult = true;
            break;
          }
          delay(100);
          // Feed watchdog during connection attempt
          pourSystem.checkWatchdog();
        }

        if (connectionResult) {
          Serial.println("✅ ThingsBoard connected!");
          thingsBoardConnected = true;
          ledController.setState(STATE_TB_CONNECTED);

          // Subscribe to RPC commands
          if (rpc.RPC_Subscribe(callbacks + 0U, callbacks + COUNT_OF(callbacks))) {
            Serial.println("✅ RPC subscriptions successful!");
            rpcSubscribed = true;

            // Send initial attributes
            tb.sendAttributeData(TB_CUP_SIZE_ATTR, 0);
            tb.sendAttributeData(TB_ML_PER_PULSE_ATTR, pourSystem.getMlPerPulse());
          } else {
            Serial.println("❌ RPC subscription failed!");
          }
        } else {
          Serial.println("❌ ThingsBoard connection failed, retrying...");
          Serial.print("WiFi Status: ");
          Serial.println(WiFi.status());
          Serial.print("IP Address: ");
          Serial.println(WiFi.localIP());
          ledController.setState(STATE_TB_FAILED);
        }
        lastConnectionAttempt = millis();
      }
    } else {
      // Already connected, run normal loop
      tb.loop();
    }
  } else {
    // WiFi disconnected, reset ThingsBoard connection
    thingsBoardConnected = false;
    rpcSubscribed = false;
    ledController.setState(STATE_WIFI_FAILED);
  }

  // Check system watchdog
  pourSystem.checkWatchdog();

  // Check network status and handle reconnections
  networkManager.handleWifiStatusChange();

  // Update pour system (includes safety checks and pour logic)
  pourSystem.update();

  // Handle pour state changes and LED updates
  static bool lastPourState = false;
  bool currentPourState = pourSystem.getIsPouring();

  // Update pour LED status
  if (currentPourState) {
    ledController.setState(STATE_POURING);
  } else if (lastPourState && !currentPourState) {
    // Pour just completed - temporarily show completion
    ledController.setTemporaryState(STATE_POUR_COMPLETE, 3000);
  }

  // Send cup size reset to ThingsBoard when pour completes
  if (thingsBoardConnected) {
    if (lastPourState && !currentPourState) {
      tb.sendAttributeData(TB_CUP_SIZE_ATTR, 0);
      Serial.println("📱 Cup size reset to 0 after pour completion");
    }
  }

  lastPourState = currentPourState;

  // Check for WiFi reset via serial command
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    command.trim();
    if (command.equalsIgnoreCase("reset_wifi")) {
      Serial.println("🔄 WiFi reset requested via serial");
      ledController.setTemporaryState(STATE_WIFI_PORTAL_ACTIVE, 2000);
      wifiManager.resetSettings();
      Serial.println("📝 WiFi settings cleared, restarting...");
      delay(1000);
      ESP.restart();
    }
  }

  // Small delay to prevent overwhelming the system
  delay(100);
}

// ThingsBoard RPC callback handlers

void processCupSizeChange(const JsonVariantConst &data, JsonDocument &response) {
  int value = data.as<int>();
  Serial.print("📱 ThingsBoard received cup size: ");
  Serial.print(value);
  Serial.println("ml");
  pourSystem.handleCupSizeChange(value);

  // Send attribute update to ThingsBoard
  if (thingsBoardConnected) {
    tb.sendAttributeData(TB_CUP_SIZE_ATTR, value);
    Serial.println("📱 Cup size attribute sent to ThingsBoard: " + String(value) + "ml");
  }

  response.set(value);
}

void processMlPerPulseChange(const JsonVariantConst &data, JsonDocument &response) {
  float value = data.as<float>();
  pourSystem.handleMlPerPulseChange(value);

  // Send attribute update to ThingsBoard
  if (thingsBoardConnected) {
    tb.sendAttributeData(TB_ML_PER_PULSE_ATTR, value);
    Serial.println("📱 ML per pulse attribute sent to ThingsBoard: " + String(value));
  }

  response.set(value);
}

void processStopCommand(const JsonVariantConst &data, JsonDocument &response) {
  int value = data.as<int>();
  if (value == 1)  // Button pressed (only act on press, not release)
  {
    pourSystem.emergencyStop();
    // Flash error LED briefly to indicate emergency stop
    ledController.setTemporaryState(STATE_ERROR, 1000);

    // Reset cup size display on dashboard
    if (thingsBoardConnected) {
      tb.sendAttributeData(TB_CUP_SIZE_ATTR, 0);
      Serial.println("📱 Dashboard cup size reset to 0");
    }
  }
  response.set("stopped");
}

void processWiFiResetCommand(const JsonVariantConst &data, JsonDocument &response) {
  int value = data.as<int>();
  if (value == 1)  // Button pressed (only act on press, not release)
  {
    Serial.println("🔄 WiFi reset requested from ThingsBoard");

    // Indicate WiFi reset in progress
    ledController.setTemporaryState(STATE_WIFI_PORTAL_ACTIVE, 2000);

    // Reset WiFi settings and restart
    wifiManager.resetSettings();
    Serial.println("📝 WiFi settings cleared, restarting...");

    delay(1000);
    ESP.restart();
  }
  response.set("wifi_reset");
}
