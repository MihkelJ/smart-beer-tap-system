#ifndef CONFIG_H
#define CONFIG_H

// ========================================
// BEER TAP CONFIGURATION TEMPLATE
// ========================================
//
// Instructions:
// 1. Copy this file to src/config.h
// 2. Replace ThingsBoard credentials with your actual values
// 3. WiFi configuration is handled automatically by WiFiManager
// 4. Never commit the actual config.h file to version control
//

// ThingsBoard credentials - KEEP THESE SECRET!
// Get these from your ThingsBoard instance
#define THINGSBOARD_SERVER \
  "YOUR_THINGSBOARD_SERVER_HERE"  // e.g., "demo.thingsboard.io" or "your-instance.com"
#define THINGSBOARD_ACCESS_TOKEN "YOUR_ACCESS_TOKEN_HERE"  // e.g., "your-device-access-token"


// ========================================
// SETUP INSTRUCTIONS
// ========================================
//
// THINGSBOARD SETUP:
// 1. Create account at https://thingsboard.io or deploy your own instance
// 2. Create new device in ThingsBoard
// 3. Copy the device access token
// 4. Create dashboard with widgets:
//    - RPC Button for Cup Size (setCupSize)
//    - Timeseries Chart for Status
//    - RPC Button for Calibration (setMlPerPulse)
//    - RPC Button for Stop (stopPour)
//    - RPC Button for WiFi Reset (resetWiFi)
//    - Gauge for Ready/Busy Status
// 5. Set up telemetry keys: cupSize, status, mlPerPulse, ready
// 6. Set up RPC commands: setCupSize, setMlPerPulse, stopPour, resetWiFi
//
// WIFI SETUP (AUTOMATIC):
// 1. Power on device - it will create "BeerTap-Setup" WiFi network
// 2. Connect to this network with your phone/laptop
// 3. Browser will open automatically to configuration page
// 4. Select your WiFi network and enter password
// 5. Use 2.4GHz WiFi network (ESP32 doesn't support 5GHz)
// 6. Ensure network allows internet access for ThingsBoard
// 7. WiFi credentials are stored securely on device
//

#endif  // CONFIG_H
