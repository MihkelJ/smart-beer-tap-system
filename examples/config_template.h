#ifndef CONFIG_H
#define CONFIG_H

// ========================================
// BEER TAP CONFIGURATION TEMPLATE
// ========================================
//
// Instructions:
// 1. Copy this file to src/config.h
// 2. Replace all placeholder values with your actual credentials
// 3. Never commit the actual config.h file to version control
//

// ThingsBoard credentials - KEEP THESE SECRET!
// Get these from your ThingsBoard instance
#define THINGSBOARD_SERVER "YOUR_THINGSBOARD_SERVER_HERE" // e.g., "demo.thingsboard.io" or "your-instance.com"
#define THINGSBOARD_ACCESS_TOKEN "YOUR_ACCESS_TOKEN_HERE" // e.g., "your-device-access-token"

// WiFi credentials - KEEP THESE SECRET!
// Your home/office WiFi network credentials
#define WIFI_SSID "YOUR_WIFI_SSID_HERE"         // e.g., "MyHomeWiFi"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD_HERE" // e.g., "mySecurePassword123"

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
//    - Gauge for Ready/Busy Status
// 5. Set up telemetry keys: cupSize, status, mlPerPulse, ready
// 6. Set up RPC commands: setCupSize, setMlPerPulse, stopPour
//
// WIFI SETUP:
// 1. Use 2.4GHz WiFi network (ESP32 doesn't support 5GHz)
// 2. Ensure network allows internet access for ThingsBoard
// 3. Test WiFi credentials on another device first
//

#endif // CONFIG_H
