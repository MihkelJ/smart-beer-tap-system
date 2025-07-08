# Bill of Materials (BOM)

## Core Components

| Component | Quantity | Description | Pin Connection |
|-----------|----------|-------------|----------------|
| ESP32 Dev Board | 1 | Main microcontroller | - |
| Flow Sensor | 1 | Water/liquid flow sensor (450 pulses/liter) | Pin 27 |
| Relay Module | 1 | 5V relay for valve control | Pin 13 |
| LED | 1 | Status indicator LED | Pin 2 |
| Solenoid Valve | 1 | Beer tap valve | Relay output |

## Power Supply

| Component | Quantity | Description |
|-----------|----------|-------------|
| 12V Power Supply | 1 | For solenoid valve |
| 5V Power Supply | 1 | For ESP32 and relay |

## Connections

| Component | ESP32 Pin | Notes |
|-----------|-----------|-------|
| Flow Sensor | GPIO 27 | Pull-up enabled, interrupt-driven |
| Relay Control | GPIO 13 | Controls solenoid valve |
| Status LED | GPIO 2 | Visual feedback |
| Ground | GND | Common ground for all components |
| 5V Supply | VIN/5V | Power for ESP32 |

## Recommended Specifications

### Flow Sensor
- **Type**: Hall effect or optical
- **Pulse Rate**: 450 pulses/liter (adjustable in software)
- **Operating Voltage**: 3.3V or 5V
- **Connection**: 3-wire (VCC, GND, Signal)

### Solenoid Valve
- **Type**: Normally closed
- **Operating Voltage**: 12V DC
- **Current**: < 1A
- **Thread Size**: Match your beer line fittings

### Relay Module
- **Type**: Single channel, optically isolated
- **Control Voltage**: 3.3V/5V compatible
- **Load Rating**: > 1A at 12V DC
- **Features**: LED indicator, flyback diode protection

## Assembly Notes

1. **Power isolation**: Use separate 12V supply for solenoid valve
2. **Ground connections**: Ensure common ground between ESP32 and relay
3. **Flow sensor mounting**: Install in-line with beer flow path
4. **Valve positioning**: Install solenoid valve in beer line before tap
5. **Enclosure**: Use food-safe enclosure for electronics near beer