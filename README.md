# 🍺 Smart Beer Tap System

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Platform: ESP32](https://img.shields.io/badge/Platform-ESP32-blue.svg)](https://www.espressif.com/en/products/socs/esp32)
[![Framework: Arduino](https://img.shields.io/badge/Framework-Arduino-00979D.svg)](https://www.arduino.cc/)
[![IoT: ThingsBoard](https://img.shields.io/badge/IoT-ThingsBoard-FF6A00.svg)](https://thingsboard.io/)

A complete automated beer dispensing system that combines ESP32-based hardware control with blockchain payment integration. The system consists of two main parts:

1. **Hardware Control**: ESP32-based beer tap controller with precise volume control and safety features
2. **Payment Integration**: Webhook service that processes blockchain payments and triggers the beer tap

## 🚀 Quick Start

**New to the project? Start here:**

1. **📖 [Read the Setup Guide](SETUP.md)** - Complete step-by-step instructions
2. **⚙️ Configure your credentials—**Copy and edit `examples/config_template.h`
3. **📱 Create ThingsBoard device—**Follow the guide for easy setup
4. **🔧 Upload code—**The system will validate your config automatically

**Setup validation built-in!** The system checks your configuration and provides clear error messages through serial output.

---

## Table of Contents

- [Quick Start](#-quick-start)  
- [System Overview](#-system-overview)
- [Features](#-features)
- [Hardware Requirements](#-hardware-requirements)
- [Software Requirements](#-software-requirements)
- [Setup Process](#-setup-process)
- [Troubleshooting](#-troubleshooting)
- [Safety Features](#-safety-features)
- [Code Architecture](#-code-architecture)
- [Payment Integration](#-payment-integration)
- [Contributing](#-contributing)

## 🔄 System Overview

This project is a complete solution for automated beer dispensing with blockchain payment integration. It consists of:

### Hardware Component

- ESP32-based controller for precise beer dispensing
- Flow sensor for volume measurement
- Relay control for tap operation
- Simple, reliable operation without visual indicators

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
- **📲 Remote Control**: Full control through ThingsBoard IoT platform
- **⚙️ Calibration**: Adjustable flow sensor calibration (ml per pulse)
- **🔄 Automatic Reconnection**: Wi-Fi and ThingsBoard connection monitoring

## 🔧 Hardware Requirements

- ESP32 microcontroller
- Flow sensor compatible with pulse counting
- Relay module (for solenoid valve control)
- Wi-Fi connectivity
- Power supply (5 V for ESP32, the appropriate voltage for relay/valve)

## 💻 Software Requirements

- Arduino IDE or PlatformIO
- ThingsBoard IoT platform account
- Required libraries:
  - Wi-Fi.h (built-in)
  - Arduino_MQTT_Client
  - ThingsBoard

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

- ❌ **Configuration error detected** = Check serial monitor for detailed error messages
- ✅ **Setup successful** = System ready for operation
- 📊 **Serial Monitor** = Detailed error messages and instructions

**No more guessing!** The system tells you exactly what's wrong and how to fix it through the serial monitor.

## 🚨 Troubleshooting

### Common Issues

**"Config Error" Message:**
- Copy template: `cp examples/config_template.h src/config.h`
- Edit with real credentials (no placeholder text)
- Make sure all fields are filled

**Wi-Fi Won't Connect:**
- Use 2.4 GHz network only (ESP32 limitation)
- Check SSID/password spelling
- Move ESP32 closer to the router

**ThingsBoard Won't Connect:**
- Verify the device access token is correct
- Check device is created in ThingsBoard
- Ensure internet connection works

🔧 **Need more help?** See the detailed [SETUP.md](SETUP.md) guide or create an issue with your Serial Monitor output.

## 🛡️ Safety Features

The system includes several safety mechanisms:

- Automatic shutoff after 90 seconds
- Maximum volume limit of 2 liters
- Connection monitoring with automatic reconnection
- Input validation for all parameters

## 📖 Usage

1. Power on the system
2. Connect to ThingsBoard (automatic)
3. Send RPC commands through ThingsBoard:
   - `setCupSize` - Set desired volume (50ml - 2000ml)
   - `setMlPerPulse` - Calibrate flow sensor
   - `stopPour` - Emergency stop
4. Monitor pour status through ThingsBoard telemetry
5. System will automatically stop when the target volume is reached

### Normal Startup Sequence

When the system boots up normally, you should see this in the serial monitor:

1. **System Start**: "🍺 Smart Beer Tap System Starting..."
2. **Config Validation**: Configuration validation messages
3. **Wi-Fi Connection**: Wi-Fi connection status and IP address
4. **ThingsBoard Connection**: Connection attempts and success confirmation
5. **System Ready**: "✅ SETUP COMPLETE! 🍺 Smart Beer Tap ready for operation"

**Total startup time**: Typically 10–30 seconds depending on network conditions.

## 📊 Status Monitoring

The system provides status information through:

### Serial Monitor (115200 baud)
- Detailed startup sequence
- Configuration validation results
- Network connection status
- Pour progress and completion
- Error messages with solutions

### ThingsBoard Telemetry
- `cupSize` - Current target volume
- `ready` - System ready status
- `mlPerPulse` - Current flow sensor calibration

## 📱 ThingsBoard Integration

### Device Configuration

The system sends the following telemetry to ThingsBoard:

| Attribute    | Type    | Range     | Description             |
|--------------|---------|-----------|-------------------------|
| `cupSize`    | Integer | 0-2000 ml | Target pour volume      |
| `ready`      | Integer | 0 or 1    | System ready status     |
| `mlPerPulse` | Float   | 0.5-10.0  | Flow sensor calibration |

### RPC Commands

| Command         | Parameters        | Description            |
|-----------------|-------------------|------------------------|
| `setCupSize`    | Integer (50-2000) | Set target pour volume |
| `setMlPerPulse` | Float (0.5-10.0)  | Calibrate flow sensor  |
| `stopPour`      | Integer (1)       | Emergency stop         |

## 💻 Code Architecture

The project consists of the following modular components:

```
src/
├── config.h              # Configuration file (user credentials)
├── constants.h           # System constants and ThingsBoard keys
├── pour_system.h/.cpp    # Core pouring logic and safety features
├── network_manager.h/.cpp # WiFi and connection management
├── config_validator.h/.cpp # Configuration validation
└── beer-tap.ino          # Main Arduino sketch
```

### Key Features:
- **Modular Architecture**: Each component has specific responsibilities
- **Safety First**: Multiple safety checks and automatic shutoffs
- **Input Validation**: All parameters validated before use
- **Error Handling**: Comprehensive error detection and reporting

## 💳 Payment Integration

The system integrates with blockchain payments through the [yodl-store-webhook](https://github.com/MihkelJ/yodl-store-webhook) service. This component:

- Processes blockchain transaction notifications
- Validates payment details
- Triggers the beer tap through ThingsBoard RPC calls
- Supports multiple beer taps with different configurations

### Setup Steps

1. Deploy the webhook service
2. Configure your beer tap in the webhook's environment variables:
   ```env
   BEER_TAPS='[
     {
       "id": "beer-tap-0",
       "transactionReceiverEns": "0xtam.eth",
       "transactionMemo": "",
       "transactionCurrency": "USD",
       "transactionAmount": "0.92",
       "thingsBoardDeviceId": "YOUR_THINGSBOARD_DEVICE_ID",
       "thingsBoardCupSize": 200,
       "thingsBoardServerUrl": "https://thingsboard.cloud"
     }
   ]'
   ```
3. Set up the ThingsBoard device ID and cup size mapping

## 🤝 Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

1. Fork the repository
2. Create your feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit your changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request


## 🔗 Related Projects

### [yodl-store-webhook](https://github.com/MihkelJ/yodl-store-webhook)

This project can be integrated with the [yodl-store-webhook](https://github.com/MihkelJ/yodl-store-webhook) to enable blockchain-based payments for beer dispensing. The webhook service:

- Processes blockchain transaction notifications
- Validates payment details
- Triggers the beer tap through ThingsBoard RPC calls
- Supports multiple beer taps with different configurations

To integrate with the webhook service:

1. Deploy the webhook service
2. Configure your beer tap with your specific details:
   - Replace `transactionReceiverEns` with your ENS name or wallet address
   - Set your desired `transactionAmount` and `transactionCurrency`
   - Use your ThingsBoard device ID from the device details page
   - Configure `thingsBoardCupSize` for the volume to dispense (in ml)
   - Set `thingsBoardServerUrl` to your ThingsBoard instance URL
3. The webhook will automatically send `setCupSize` RPC commands to trigger pours