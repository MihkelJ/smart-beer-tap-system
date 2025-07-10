# WiFi Provisioning Implementation

## Overview
The beer tap system now uses WiFiManager for dynamic WiFi configuration, eliminating the need for hardcoded credentials and enabling easy deployment in any environment.

## Key Features

### 🌐 Captive Portal
- **Access Point Name**: `BeerTap-Setup`
- **Portal Address**: `192.168.4.1` (automatic redirect)
- **Timeout**: 3 minutes for configuration
- **Custom Branding**: "🍺 Beer Tap WiFi Setup" with dark mode

### 🚨 LED Status Indicators
The single LED provides visual feedback during WiFi provisioning:

- **Double Blink**: WiFi portal active, waiting for connection
- **Triple Blink**: User actively configuring WiFi
- **Fast Blink**: Connecting to WiFi network
- **Heartbeat**: WiFi connected successfully
- **Slow Blink**: WiFi connection failed

### ⚙️ Custom Configuration
- **Optional ThingsBoard Server**: Override default server address
- **Persistent Storage**: WiFi credentials saved in ESP32 flash
- **Non-blocking Operation**: Maintains real-time pour system performance

## Setup Process

### Initial Configuration
1. **Power on the device** - LED shows fast blink (booting)
2. **Look for "BeerTap-Setup" WiFi network** - LED shows double blink
3. **Connect to the network** with any WiFi device
4. **Browser opens automatically** to configuration page
5. **Select your WiFi network** and enter password
6. **Optionally configure** custom ThingsBoard server
7. **Save configuration** - LED shows fast blink while connecting
8. **Success** - LED shows heartbeat pattern when connected

### Configuration Interface
- Network scanning with signal strength indicators
- Responsive design for mobile and desktop
- Clear error messages and status feedback
- Dark mode for better visibility

## Reset Options

### 1. ThingsBoard Dashboard
- Add a new RPC button with command `resetWiFi`
- Click to remotely reset WiFi settings and restart device
- Device will restart in provisioning mode

### 2. Serial Command
- Connect via USB/serial monitor
- Send command: `reset_wifi`
- Device will clear settings and restart

### 3. Physical Reset (Future Enhancement)
- Hold reset button for 10 seconds during boot
- GPIO pin configuration can be added for field reset

## WiFi Manager Library

### Installation
Install via Arduino Library Manager:
- Search for "WiFiManager" by Tzapu
- Install the latest version
- Dependencies are handled automatically

### Library Features Used
- **autoConnect()**: Automatic connection to saved networks
- **Captive Portal**: Automatic redirection to config page
- **Custom Parameters**: ThingsBoard server override
- **Callbacks**: LED status updates during configuration
- **Non-blocking Mode**: Maintains system responsiveness

## Benefits

### For Deployment
- ✅ Zero-touch WiFi setup in any location
- ✅ No firmware changes needed for different networks
- ✅ User-friendly interface for non-technical users
- ✅ Remote reconfiguration capability

### For Development
- ✅ No hardcoded credentials in source code
- ✅ Easy testing across different networks
- ✅ Maintains all existing safety systems
- ✅ Preserves pour system timing and accuracy

### For Security
- ✅ Credentials stored locally on device
- ✅ No credentials in version control
- ✅ Open source WiFiManager library
- ✅ Optional custom server configuration

## Technical Implementation

### Files Modified
- **beer-tap.ino**: Replaced hardcoded WiFi with WiFiManager
- **src/config.h**: Removed WiFi credentials, updated documentation
- **src/constants.h**: Added WiFi provisioning constants and RPC command
- **src/led_controller.h/.cpp**: Added WiFi provisioning states

### Key Functions
- **autoConnect()**: Handles WiFi connection with fallback to portal
- **process()**: Non-blocking WiFi manager processing in main loop
- **resetSettings()**: Clears saved credentials for reconfiguration
- **Custom callbacks**: LED status updates and parameter handling

### Backward Compatibility
- ✅ All ThingsBoard integration preserved
- ✅ Pour system functionality unchanged
- ✅ Safety systems and watchdog maintained
- ✅ LED patterns enhanced but compatible

## Troubleshooting

### Portal Not Appearing
1. Check if "BeerTap-Setup" network is visible
2. LED should show double blink when portal active
3. Try connecting directly to 192.168.4.1
4. Check portal timeout (3 minutes)

### Connection Failures
1. Verify 2.4GHz network (ESP32 doesn't support 5GHz)
2. Check network password accuracy
3. Ensure network allows internet access for ThingsBoard
4. Use serial monitor for detailed connection logs

### Reset Issues
1. Try serial command: `reset_wifi`
2. Power cycle device if unresponsive
3. Check serial output for error messages
4. Verify WiFiManager library installation

## Future Enhancements

### Potential Additions
- **Physical reset button**: GPIO-based reset for field use
- **SPIFFS configuration**: Persistent custom parameters
- **WPS support**: Push-button WiFi setup
- **Multiple network support**: Fallback networks
- **Status dashboard**: Web interface for device status
- **OTA updates**: Firmware updates via portal

### Advanced Features
- **Enterprise WiFi**: WPA2-Enterprise support
- **Static IP**: Manual IP configuration
- **DNS settings**: Custom DNS servers
- **Time zone**: Automatic time configuration
- **Device naming**: Custom device identification