# 🍺 Smart Beer Tap System

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Platform: ESP32](https://img.shields.io/badge/Platform-ESP32-blue.svg)](https://www.espressif.com/en/products/socs/esp32)
[![Framework: Arduino](https://img.shields.io/badge/Framework-Arduino-00979D.svg)](https://www.arduino.cc/)
[![IoT: Blynk](https://img.shields.io/badge/IoT-Blynk-00A0E0.svg)](https://blynk.io/)

A complete automated beer dispensing system that combines ESP32-based hardware control with blockchain payment integration. The system consists of two main components:

1. **Hardware Control**: ESP32-based beer tap controller with precise volume control and safety features
2. **Payment Integration**: Webhook service that processes blockchain payments and triggers the beer tap

## 🚀 Quick Start

**New to the project? Start here:**

1. **📖 [Read the Setup Guide](SETUP.md)** - Complete step-by-step instructions
2. **⚙️ Configure your credentials** - Copy and edit `examples/config_template.h`
3. **📱 Create Blynk app** - Follow the guide for easy setup
4. **🔧 Upload code** - The system will validate your config automatically

**Setup validation built-in!** The system checks your configuration and provides clear error messages with LED feedback.

---

## Table of Contents

- [Quick Start](#-quick-start)  
- [System Overview](#-system-overview)
- [Features](#-features)
- [Hardware Requirements](#-hardware-requirements)
- [Software Requirements](#-software-requirements)
- [Setup Process](#-setup-process)
- [Status Indicators](#-status-indicators)
- [Troubleshooting](#-troubleshooting)
- [Safety Features](#-safety-features)
- [Code Architecture](#-code-architecture)
- [Payment Integration](#-payment-integration)
- [Contributing](#-contributing)
- [License](#-license)

## 🔄 System Overview

This project is a complete solution for automated beer dispensing with blockchain payment integration. It consists of:

### Hardware Component

- ESP32-based controller for precise beer dispensing
- Flow sensor for volume measurement
- Relay control for tap operation
- LED indicators for system status

### Payment Component

- [yodl-store-webhook](https://github.com/MihkelJ/yodl-store-webhook) service
- Blockchain transaction processing
- Automatic tap activation on successful payment
- Support for multiple payment configurations

## ✨ Features

- **🎯 Precise Volume Control**: Set specific cup sizes for accurate pours
- **📊 Flow Monitoring**: Real-time tracking of poured volume using a flow sensor
- **🛡️ Safety Features**:
  - Maximum pour time limit (90 seconds)
  - Maximum volume limit (2 liters)
  - Automatic shutoff when target volume is reached
- **🎯 Standard Pouring Mode**: Preset cup sizes from 50ml to 2000ml for precise dispensing
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
- Power supply (12V and 5V)

## 💻 Software Requirements

- Arduino IDE
- Blynk IoT platform
- Required libraries:
  - WiFi.h
  - BlynkSimpleEsp32.h

## ⚙️ Setup Process

### Option 1: Guided Setup (Recommended)

📖 **[Follow the complete SETUP.md guide](SETUP.md)** for step-by-step instructions with screenshots and troubleshooting.

### Option 2: Quick Setup

For experienced users:

```bash
# 1. Copy config template
cp examples/config_template.h src/config.h

# 2. Edit with your credentials  
nano src/config.h

# 3. Upload to ESP32
# The system will validate your config automatically!
```

### Built-in Validation ✅

The system includes **automatic configuration validation**:

- ❌ **Ultra-fast red blinking** = Configuration error detected
- ✅ **4 quick blinks** = Setup successful, system ready
- 📊 **Serial Monitor** = Detailed error messages and instructions

**No more guessing!** The system tells you exactly what's wrong and how to fix it.

## 🚨 Troubleshooting

### LED Error Patterns

| LED Pattern | Problem | Solution |
|-------------|---------|----------|
| ⚡ **Ultra-fast blinking** | Config file error | Check `src/config.h` - follow [SETUP.md](SETUP.md) |
| 🔄 **Medium continuous blink** | WiFi/Blynk connecting | Wait 30s, check credentials |
| 🚫 **No LED activity** | Power/hardware issue | Check wiring, power supply |

### Common Issues

**"Config Error" Message:**
- Copy template: `cp examples/config_template.h src/config.h`
- Edit with real credentials (no placeholder text)
- Make sure all fields are filled

**WiFi Won't Connect:**
- Use 2.4GHz network only (ESP32 limitation)
- Check SSID/password spelling
- Move ESP32 closer to router

**Blynk Won't Connect:**
- Verify auth token is correct
- Check template ID matches web console
- Ensure internet connection works

🔧 **Need more help?** See the detailed [SETUP.md](SETUP.md) guide or create an issue with your Serial Monitor output.

## 🛡️ Safety Features

The system includes several safety mechanisms:

- Automatic shutoff after 90 seconds
- Maximum volume limit of 2 liters
- Visual and status indicators for system state
- Connection monitoring with automatic reconnection

## 📖 Usage

1. Power on the system
2. Connect to the Blynk app
3. Select desired cup size (50ml - 2000ml)
4. Monitor pour status through the app
5. System will automatically stop when target volume is reached

### Normal Startup Sequence

When the system boots up normally, you should see this LED pattern sequence:

1. **System Start**: 1 long blink (500ms on, 500ms off) - indicates the ESP32 is starting up
2. **Connecting**: Medium speed continuous blinking (300ms intervals) - system is connecting to WiFi and Blynk
3. **WiFi Connected**: 2 quick blinks - WiFi connection established
4. **Blynk Connected**: 2 quick blinks with pause - Blynk connection established  
5. **System Ready**: 4 quick blinks - initialization complete, all systems operational
6. **Ready State**: Slow blink every 2 seconds - normal operational state, ready for commands

**Total startup time**: Typically 10-30 seconds depending on network conditions.

### Troubleshooting Startup Issues

If the startup sequence doesn't complete as expected:

#### **Stuck on "Connecting" pattern** (continuous medium blinking)
- **Issue**: Cannot connect to WiFi or Blynk
- **Check**: 
  - WiFi credentials in code are correct
  - WiFi network is available and working
  - Blynk auth token is valid
  - Internet connection is stable
- **Solution**: Verify credentials, check network, restart router if needed

#### **No WiFi Connected pattern** (missing 2 quick blinks)
- **Issue**: WiFi connection failed
- **Check**: WiFi SSID and password in code
- **Solution**: Update WiFi credentials and re-upload code

#### **No Blynk Connected pattern** (missing 2 quick blinks with pause)
- **Issue**: Blynk service connection failed
- **Check**: 
  - Blynk auth token is correct
  - Blynk device is online in the app
  - Internet connection allows HTTPS traffic
- **Solution**: Verify Blynk credentials, check firewall settings

#### **System never reaches Ready state** (no slow blinking)
- **Issue**: Initialization failed
- **Check**: Serial monitor for error messages
- **Solution**: Check hardware connections, restart system

#### **Error patterns during startup**
- **Input Error** (short-long-short): Invalid configuration detected
- **Network Error** (long-long-short): Network connectivity issues
- **Critical Error** (very fast blinking): Hardware or software malfunction

**Serial Monitor**: Always check the serial monitor (115200 baud) for detailed error messages and debugging information during startup.

## 📊 Status Indicators

The system uses an advanced LED pattern system to provide clear visual feedback about the current state and any issues:

### LED Pattern System

#### **Background Patterns** (Continuous)
- **Ready State**: Slow blink every 2 seconds - system is ready for operation
- **Pouring State**: Fast toggle every 250ms - actively dispensing beer

#### **Event Patterns** (One-time notifications)
- **System Start**: 1 long blink - system is starting up
- **System Ready**: 4 quick blinks - system initialization complete
- **Pour Complete**: 3 medium blinks - dispensing finished successfully
- **Cup Size Change**: 2 medium blinks - new cup size selected

#### **Connection Patterns**
- **WiFi Connected**: 2 quick blinks - WiFi connection established
- **Blynk Connected**: 2 quick blinks with pause - Blynk connection established

#### **Error Patterns** (Repeating until resolved)
- **Critical Error**: Very fast continuous blink (100ms) - system malfunction
- **Warning**: Medium speed blink (300ms) - non-critical issues
- **Input Error**: Short-long-short sequence - invalid settings detected
- **Timeout Error**: Long-short-long sequence - pour timeout occurred
- **Volume Error**: Short-short-long sequence - maximum volume exceeded
- **Network Error**: Long-long-short sequence - WiFi/Blynk connection issues
- **Sensor Error**: Short-long-long sequence - flow sensor problems

#### **Pattern Priority**
- Error patterns interrupt background patterns
- Event patterns play once then return to background
- Critical errors take precedence over warnings
- System maintains visual feedback even during network issues

### Additional Status Sources
- Blynk app provides real-time status updates
- Serial monitor shows detailed system statistics and error logs

## 📱 Blynk Integration

### Widget Configuration

| Widget               | Type          | Range            | Description             |
| -------------------- | ------------- | ---------------- | ----------------------- |
| Cup Size (V1)        | Value Display | 0-2000 ml        | Target pour volume      |
| Status (V2)          | Label         | Text             | System status display   |
| Calibration (V3)     | Value Display | 1.0-5.0 ml/pulse | Flow sensor calibration |

### Setup Instructions

1. Create a new device in Blynk IoT
2. Add the widgets as specified in the table above
3. Configure each widget with the following settings:
   - Cup Size: Step size 50ml, Color #2196F3, Range 0-2000ml
   - Status: Large font, Color #4CAF50
   - Calibration: Step size 0.1, Color #FF9800, Range 0.5-10.0

## 💻 Code

The project consists of the following files:

- [`main.ino`](main.ino) - Main Arduino sketch containing the core functionality
  - Flow sensor integration
  - Blynk communication
  - Pour control logic
  - Safety features implementation

To get started with the code:

1. Clone the repository
2. Open `main.ino` in Arduino IDE
3. Install required libraries
4. Configure your credentials
5. Upload to ESP32

## 💳 Payment Integration

The system integrates with blockchain payments through the [yodl-store-webhook](https://github.com/MihkelJ/yodl-store-webhook) service. This component:

- Processes blockchain transaction notifications
- Validates payment details
- Triggers the beer tap through Blynk integration
- Supports multiple beer taps with different configurations

### Setup Steps

1. Deploy the webhook service
2. Configure your beer tap in the webhook's environment variables:
   ```env
   BEER_TAPS='[
     {
       "transactionReceiverEns": "your.ens.name",
       "transactionMemo": "Beer",
       "transactionCurrency": "EUR",
       "transactionAmount": "4.2",
       "blynkDeviceToken": "YOUR_BLYNK_TOKEN",
       "blynkDevicePin": "V1",
       "blynkDevicePinValue": "500"
     }
   ]'
   ```
3. Set up the Blynk device token and pin mapping

## 🤝 Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

1. Fork the repository
2. Create your feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit your changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 🔗 Related Projects

### [yodl-store-webhook](https://github.com/MihkelJ/yodl-store-webhook)

This project can be integrated with the [yodl-store-webhook](https://github.com/MihkelJ/yodl-store-webhook) to enable blockchain-based payments for beer dispensing. The webhook service:

- Processes blockchain transaction notifications
- Validates payment details
- Triggers the beer tap through Blynk integration
- Supports multiple beer taps with different configurations

To integrate with the webhook service:

1. Deploy the webhook service
2. Configure your beer tap in the webhook's environment variables
3. Set up the Blynk device token and pin mapping
