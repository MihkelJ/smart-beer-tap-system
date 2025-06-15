# 🍺 Smart Beer Tap System

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Platform: ESP32](https://img.shields.io/badge/Platform-ESP32-blue.svg)](https://www.espressif.com/en/products/socs/esp32)
[![Framework: Arduino](https://img.shields.io/badge/Framework-Arduino-00979D.svg)](https://www.arduino.cc/)
[![IoT: Blynk](https://img.shields.io/badge/IoT-Blynk-00A0E0.svg)](https://blynk.io/)

A complete automated beer dispensing system that combines ESP32-based hardware control with blockchain payment integration. The system consists of two main components:

1. **Hardware Control**: ESP32-based beer tap controller with precise volume control and safety features
2. **Payment Integration**: Webhook service that processes blockchain payments and triggers the beer tap

## Table of Contents

- [System Overview](#-system-overview)
- [Features](#-features)
- [Hardware Requirements](#-hardware-requirements)
- [Software Requirements](#-software-requirements)
- [Configuration](#-configuration)
- [Safety Features](#-safety-features)
- [Usage](#-usage)
- [Status Indicators](#-status-indicators)
- [Blynk Integration](#-blynk-integration)
- [Code](#-code)
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
- Power supply (12V and 5V)

## 💻 Software Requirements

- Arduino IDE
- Blynk IoT platform
- Required libraries:
  - WiFi.h
  - BlynkSimpleEsp32.h

## ⚙️ Configuration

Before using the system, you need to configure:

1. Blynk credentials:

   ```cpp
   #define BLYNK_TEMPLATE_ID "your_template_id"
   #define BLYNK_TEMPLATE_NAME "your_template_name"
   #define BLYNK_AUTH_TOKEN "your_auth_token"
   ```

2. WiFi credentials:
   ```cpp
   const char* ssid = "your_ssid";
   const char* password = "your_password";
   ```

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

## 📱 Blynk Integration

### Widget Configuration

| Widget               | Type          | Range            | Description             |
| -------------------- | ------------- | ---------------- | ----------------------- |
| Cup Size (V1)        | Value Display | 0-2000 ml        | Target pour volume      |
| Status (V2)          | Label         | Text             | System status display   |
| Calibration (V3)     | Value Display | 1.0-5.0 ml/pulse | Flow sensor calibration |
| Continuous Pour (V4) | Switch        | ON/OFF           | Manual control mode     |

### Setup Instructions

1. Create a new device in Blynk IoT
2. Add the widgets as specified in the table above
3. Configure each widget with the following settings:
   - Cup Size: Step size 50ml, Color #2196F3
   - Status: Large font, Color #4CAF50
   - Calibration: Step size 0.1, Color #FF9800
   - Continuous Pour: Color #E91E63

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
