# Basic Setup Example

This folder contains a minimal example for getting started with the Smart Beer Tap System.

## Quick Start

1. **Copy configuration template:**
   ```bash
   cp ../config_template.h ../../src/config.h
   ```

2. **Edit your credentials:**
   ```bash
   nano ../../src/config.h  # or use your preferred editor
   ```

3. **Open Arduino IDE:**
   - Open `beer-tap.ino` 
   - Select board: ESP32 Dev Module
   - Compile and upload

4. **Hardware connections:**
   - LED: Pin 2
   - Relay: Pin 13  
   - Flow sensor: Pin 27
   - Power: 5V/3.3V as needed

## Blynk App Setup

Create widgets in your Blynk app:

| Widget | Virtual Pin | Settings |
|--------|-------------|----------|
| Slider | V1 | Cup Size (0-2000ml) |
| Label  | V2 | Status display |
| Slider | V3 | Calibration (0.5-10.0) |

## Testing

1. Power on the ESP32
2. Watch LED patterns:
   - Single blink: System starting
   - Continuous medium blink: Connecting
   - 2 quick blinks: WiFi connected
   - 2 quick blinks with pause: Blynk connected
   - 4 quick blinks: System ready
   - Slow blink every 2s: Ready for operation

3. Use Blynk app to test:
   - Set cup size (e.g., 500ml)
   - System should start pouring
   - Monitor status updates