#ifndef LED_CONTROLLER_H
#define LED_CONTROLLER_H

#include <Arduino.h>
#include "constants.h"

// LED patterns for different system states
enum LEDPattern {
    LED_OFF,              // LED off
    LED_SOLID,            // LED solid on - System ready
    LED_BLINK_SLOW,       // Slow blink (1 second on/off) - General error
    LED_BLINK_FAST,       // Fast blink (0.25 seconds on/off) - Booting/Connecting
    LED_DOUBLE_BLINK,     // Double blink pattern - WiFi issues
    LED_TRIPLE_BLINK,     // Triple blink pattern - Configuration error
    LED_QUADRUPLE_BLINK,  // Quadruple blink pattern - ThingsBoard issues
    LED_PULSE_SLOW,       // Slow pulse - Pour in progress
    LED_PULSE_FAST,       // Fast pulse - Pour complete
    LED_HEARTBEAT         // Heartbeat pattern - Normal operation
};

// System states for LED indication
enum SystemState {
    STATE_BOOTING,         // System starting up
    STATE_WIFI_CONNECTING, // Connecting to WiFi
    STATE_WIFI_CONNECTED,  // WiFi connected
    STATE_WIFI_FAILED,     // WiFi connection failed
    STATE_TB_CONNECTING,   // Connecting to ThingsBoard
    STATE_TB_CONNECTED,    // ThingsBoard connected
    STATE_TB_FAILED,       // ThingsBoard connection failed
    STATE_SYSTEM_READY,    // System ready for operation
    STATE_POURING,         // Pour in progress
    STATE_POUR_COMPLETE,   // Pour completed
    STATE_ERROR,           // System error
    STATE_CONFIG_ERROR     // Configuration error
};

class LEDController {
private:
    struct LEDState {
        uint8_t pin;
        LEDPattern pattern;
        bool state;
        unsigned long lastUpdate;
        uint8_t blinkCount;
        uint8_t maxBlinks;
        bool isOn;
    };
    
    LEDState led; // Single LED
    SystemState currentState;
    SystemState priorityState; // For temporary high-priority states
    unsigned long priorityStateEnd;
    
    void updateLEDPattern();
    void setLEDState(bool on);
    LEDPattern getPatternForState(SystemState state);
    
public:
    LEDController();
    void begin();
    void update();
    void setState(SystemState state);
    void setTemporaryState(SystemState state, unsigned long durationMs);
    void setOff();
    void testPattern(); // Test LED
};

#endif // LED_CONTROLLER_H