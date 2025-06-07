# 🍺 Smart Beer Tap System

An automated beer dispensing system built with ESP32 that allows precise control over beer pouring through a Blynk mobile app interface.

## ✨ Features

- **🎯 Precise Volume Control**: Set specific cup sizes for accurate pours
- **📊 Flow Monitoring**: Real-time tracking of poured volume using a flow sensor
- **🛡️ Safety Features**:
  - Maximum pour time limit (90 seconds)
  - Maximum volume limit (2 liters)
  - Automatic shutoff when target volume is reached
- **🔄 Multiple Pouring Modes**:
  - Standard mode with preset cup sizes
  - Continuous pour mode for manual control
- **📱 Status Monitoring**:
  - Real-time pour status updates
  - LED indicators for system state
  - WiFi and Blynk connection status monitoring
- **📲 Remote Control**: Full control through Blynk mobile app
- **⚙️ Calibration**: Adjustable flow sensor calibration (ml per pulse)

## 🔧 Hardware Requirements

- ESP32 microcontroller
- Flow sensor
- Relay module
- WiFi connectivity
- Power supply (v12 and v5)

## 💻 Software Requirements

- Arduino IDE
- Blynk IoT platform
- Required libraries:
  - WiFi.h
  - BlynkSimpleEsp32.h

## ⚙️ Configuration

Before using the system, you need to configure:

1. Blynk credentials:
   - Template ID: `BLYNK_TEMPLATE_ID`
   - Template Name: `BLYNK_TEMPLATE_NAME`
   - Auth Token: `BLYNK_AUTH_TOKEN`
2. WiFi credentials:
   - SSID: `ssid`
   - Password: `password`

## 🛡️ Safety Features

The system includes several safety mechanisms:

- Automatic shutoff after 90 seconds
- Maximum volume limit of 2 liters
- Visual and status indicators for system state
- Connection monitoring with automatic reconnection

## 📖 Usage

1. Power on the system
2. Connect to the Blynk app
3. Select desired cup size or enable continuous pour mode
4. Monitor pour status through the app
5. System will automatically stop when target volume is reached

## 📊 Status Indicators

- LED blinking patterns indicate different system states
- Blynk app provides real-time status updates
- Serial monitor shows detailed system statistics

## 🎮 Blynk Widgets

The system uses the following Blynk widgets for control and monitoring:

### 🎯 Control Widgets

- **Cup Size Selector** (V1)

  - Allows setting the target pour volume in milliliters
  - Set to 0 to cancel current pour
  - Automatically resets after pour completion

- **Continuous Pour Switch** (V4)
  - Toggle switch for continuous pouring mode
  - When enabled, system pours until manually stopped
  - Overrides cup size settings

### 📊 Monitoring Widgets

- **Status Display** (V2)

  - Shows current system status:
    - "System Ready"
    - "Pouring..."
    - "Pour Complete!"
    - Error messages for timeouts or volume limits

- **Flow Sensor Calibration** (V3)
  - Adjustable value for flow sensor calibration
  - Default: 2.222 ml per pulse
  - Can be fine-tuned for accurate volume measurement

## 🛠️ Blynk Widget Setup

To set up the Blynk widgets, follow these steps:

1. Create a new device in Blynk IoT
2. Add the following widgets to your device:

### Cup Size Selector (V1)

- Type: Value Display
- Input: Numeric
- Range: 0-2000
- Step: 50
- Label: "Cup Size (ml)"
- Color: #2196F3

### Status Display (V2)

- Type: Label
- Input: Text
- Label: "Status"
- Color: #4CAF50
- Font Size: Large

### Flow Sensor Calibration (V3)

- Type: Value Display
- Input: Numeric
- Range: 1.0-5.0
- Step: 0.1
- Label: "Calibration (ml/pulse)"
- Color: #FF9800

### Continuous Pour Switch (V4)

- Type: Switch
- Input: Digital
- Label: "Continuous Pour"
- Color: #E91E63
- ON/OFF Labels: "Enabled"/"Disabled"

After adding all widgets, make sure to:

1. Save your device configuration
2. Copy the device credentials to your Arduino code
3. Test each widget's functionality
