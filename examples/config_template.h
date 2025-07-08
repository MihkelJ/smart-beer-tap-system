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

// Blynk credentials - KEEP THESE SECRET!
// Get these from your Blynk.Cloud console at https://blynk.cloud
#define BLYNK_TEMPLATE_ID "YOUR_TEMPLATE_ID_HERE"      // e.g., "TMPL4f7ASCKOI"
#define BLYNK_TEMPLATE_NAME "YOUR_TEMPLATE_NAME_HERE"  // e.g., "beertap"
#define BLYNK_AUTH_TOKEN "YOUR_AUTH_TOKEN_HERE"        // e.g., "your-32-char-auth-token"

// WiFi credentials - KEEP THESE SECRET!
// Your home/office WiFi network credentials
#define WIFI_SSID "YOUR_WIFI_SSID_HERE"        // e.g., "MyHomeWiFi"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD_HERE" // e.g., "mySecurePassword123"

// ========================================
// SETUP INSTRUCTIONS
// ========================================
//
// BLYNK SETUP:
// 1. Create account at https://blynk.cloud
// 2. Create new template for "ESP32" device
// 3. Add datastreams:
//    - V1: Cup Size (Integer, 0-2000, ml)
//    - V2: Status (String)  
//    - V3: ML per Pulse (Double, 0.5-10.0)
// 4. Create mobile app with widgets:
//    - Slider for Cup Size (V1)
//    - Label for Status (V2)
//    - Slider for Calibration (V3)
// 5. Copy Template ID, Name, and Auth Token here
//
// WIFI SETUP:
// 1. Use 2.4GHz WiFi network (ESP32 doesn't support 5GHz)
// 2. Ensure network allows internet access for Blynk
// 3. Test WiFi credentials on another device first
//

#endif // CONFIG_H