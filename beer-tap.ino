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
 * - Remote control via Blynk IoT platform
 * - Comprehensive safety systems
 * - WiFi connectivity with auto-reconnection
 */

// Include configuration and modules
#include "src/config.h"
#include "src/constants.h"
#include <BlynkSimpleEsp32.h>
#include "src/led_controller.h"
#include "src/pour_system.h"
#include "src/network_manager.h"
#include "src/config_validator.h"

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
  
  // Initialize network connectivity
  networkManager.init();
  
  // Start Blynk connection
  Serial.println("📡 Connecting to Blynk...");
  Blynk.begin(BLYNK_AUTH_TOKEN, WIFI_SSID, WIFI_PASSWORD);
  
  // Initialize Blynk values to safe defaults
  networkManager.initializeBlynkValues(pourSystem.getMlPerPulse());
  Blynk.virtualWrite(CUP_SIZE_PIN, 0);
  Blynk.virtualWrite(ML_PER_PULSE_PIN, pourSystem.getMlPerPulse());
  pourSystem.updateStatus(STATUS_READY);
  
  // System ready indication
  Serial.println("");
  Serial.println("✅ SETUP COMPLETE!");
  Serial.println("🍺 Smart Beer Tap ready for operation");
  Serial.println("📱 Use your Blynk app to control the tap");
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
  
  // Normal operation - run all systems
  Blynk.run();
  
  // Check system watchdog
  pourSystem.checkWatchdog();
  
  // Update LED patterns non-blocking
  ledController.update();
  
  // Check network status and handle reconnections
  networkManager.handleWifiStatusChange();
  networkManager.handleBlynkStatusChange(pourSystem.getCurrentCupSize(), pourSystem.getMlPerPulse());
  
  // Update pour system (includes safety checks and pour logic)
  pourSystem.update();
  
  // Small delay to prevent overwhelming the system
  delay(100);
}

// Blynk event handlers - these need to be in the main file for Blynk to find them

BLYNK_WRITE(CUP_SIZE_PIN)
{
  int value = param.asInt();
  Serial.print("📱 Blynk received cup size: ");
  Serial.print(value);
  Serial.println("ml");
  pourSystem.handleCupSizeChange(value);
}

BLYNK_WRITE(ML_PER_PULSE_PIN)
{
  float value = param.asFloat();
  pourSystem.handleMlPerPulseChange(value);
}

BLYNK_WRITE(STOP_BUTTON_PIN)
{
  int value = param.asInt();
  if (value == 1)  // Button pressed (only act on press, not release)
  {
    pourSystem.emergencyStop();
    // Reset cup size display on dashboard
    Blynk.virtualWrite(CUP_SIZE_PIN, 0);
    Serial.println("📱 Dashboard cup size reset to 0");
  }
}