#include "led_controller.h"

LEDController::LEDController() {
    // Initialize single LED state
    led = {LED_SYSTEM_PIN, LED_OFF, false, 0, 0, 0, false};
    currentState = STATE_BOOTING;
    priorityState = STATE_BOOTING;
    priorityStateEnd = 0;
}

void LEDController::begin() {
    // Initialize LED pin as output
    pinMode(led.pin, OUTPUT);
    digitalWrite(led.pin, LOW);
    
    // Test pattern on startup
    testPattern();
}

void LEDController::update() {
    unsigned long currentTime = millis();
    
    // Check if priority state has expired
    if (priorityStateEnd > 0 && currentTime > priorityStateEnd) {
        priorityStateEnd = 0;
        priorityState = currentState;
    }
    
    // Use priority state if active, otherwise use current state
    SystemState activeState = (priorityStateEnd > 0) ? priorityState : currentState;
    led.pattern = getPatternForState(activeState);
    
    // Update LED pattern
    updateLEDPattern();
}

void LEDController::updateLEDPattern() {
    unsigned long currentTime = millis();
    
    switch (led.pattern) {
        case LED_OFF:
            setLEDState(false);
            break;
            
        case LED_SOLID:
            setLEDState(true);
            break;
            
        case LED_BLINK_SLOW:
            if (currentTime - led.lastUpdate >= 1000) {
                led.isOn = !led.isOn;
                setLEDState(led.isOn);
                led.lastUpdate = currentTime;
            }
            break;
            
        case LED_BLINK_FAST:
            if (currentTime - led.lastUpdate >= 250) {
                led.isOn = !led.isOn;
                setLEDState(led.isOn);
                led.lastUpdate = currentTime;
            }
            break;
            
        case LED_DOUBLE_BLINK:
            if (currentTime - led.lastUpdate >= 150) {
                if (led.blinkCount < 4) { // 2 blinks = 4 state changes
                    led.isOn = !led.isOn;
                    setLEDState(led.isOn);
                    led.blinkCount++;
                } else {
                    // Pause between double blinks
                    if (currentTime - led.lastUpdate >= 1000) {
                        led.blinkCount = 0;
                        led.isOn = false;
                        setLEDState(false);
                    }
                }
                led.lastUpdate = currentTime;
            }
            break;
            
        case LED_TRIPLE_BLINK:
            if (currentTime - led.lastUpdate >= 150) {
                if (led.blinkCount < 6) { // 3 blinks = 6 state changes
                    led.isOn = !led.isOn;
                    setLEDState(led.isOn);
                    led.blinkCount++;
                } else {
                    // Pause between triple blinks
                    if (currentTime - led.lastUpdate >= 1000) {
                        led.blinkCount = 0;
                        led.isOn = false;
                        setLEDState(false);
                    }
                }
                led.lastUpdate = currentTime;
            }
            break;
            
        case LED_QUADRUPLE_BLINK:
            if (currentTime - led.lastUpdate >= 150) {
                if (led.blinkCount < 8) { // 4 blinks = 8 state changes
                    led.isOn = !led.isOn;
                    setLEDState(led.isOn);
                    led.blinkCount++;
                } else {
                    // Pause between quadruple blinks
                    if (currentTime - led.lastUpdate >= 1000) {
                        led.blinkCount = 0;
                        led.isOn = false;
                        setLEDState(false);
                    }
                }
                led.lastUpdate = currentTime;
            }
            break;
            
        case LED_PULSE_SLOW:
            if (currentTime - led.lastUpdate >= 500) {
                led.isOn = !led.isOn;
                setLEDState(led.isOn);
                led.lastUpdate = currentTime;
            }
            break;
            
        case LED_PULSE_FAST:
            if (currentTime - led.lastUpdate >= 100) {
                led.isOn = !led.isOn;
                setLEDState(led.isOn);
                led.lastUpdate = currentTime;
            }
            break;
            
        case LED_HEARTBEAT:
            // Heartbeat pattern: quick double pulse then pause
            if (currentTime - led.lastUpdate >= 100) {
                if (led.blinkCount < 4) { // 2 quick pulses
                    led.isOn = !led.isOn;
                    setLEDState(led.isOn);
                    led.blinkCount++;
                } else {
                    // Long pause between heartbeats
                    if (currentTime - led.lastUpdate >= 1500) {
                        led.blinkCount = 0;
                        led.isOn = false;
                        setLEDState(false);
                    }
                }
                led.lastUpdate = currentTime;
            }
            break;
    }
}

void LEDController::setLEDState(bool on) {
    if (led.state != on) {
        led.state = on;
        digitalWrite(led.pin, on ? HIGH : LOW);
    }
}

LEDPattern LEDController::getPatternForState(SystemState state) {
    switch (state) {
        case STATE_BOOTING:
            return LED_BLINK_FAST;
        case STATE_WIFI_CONNECTING:
            return LED_DOUBLE_BLINK;
        case STATE_WIFI_CONNECTED:
            return LED_HEARTBEAT;
        case STATE_WIFI_FAILED:
            return LED_DOUBLE_BLINK;
        case STATE_TB_CONNECTING:
            return LED_TRIPLE_BLINK;
        case STATE_TB_CONNECTED:
            return LED_SOLID;
        case STATE_TB_FAILED:
            return LED_QUADRUPLE_BLINK;
        case STATE_SYSTEM_READY:
            return LED_SOLID;
        case STATE_POURING:
            return LED_PULSE_SLOW;
        case STATE_POUR_COMPLETE:
            return LED_PULSE_FAST;
        case STATE_ERROR:
            return LED_BLINK_SLOW;
        case STATE_CONFIG_ERROR:
            return LED_TRIPLE_BLINK;
        default:
            return LED_OFF;
    }
}

void LEDController::setState(SystemState state) {
    currentState = state;
    led.lastUpdate = millis();
    led.blinkCount = 0;
}

void LEDController::setTemporaryState(SystemState state, unsigned long durationMs) {
    priorityState = state;
    priorityStateEnd = millis() + durationMs;
    led.lastUpdate = millis();
    led.blinkCount = 0;
}

void LEDController::setOff() {
    setState(STATE_BOOTING); // Will result in LED_OFF pattern
    led.pattern = LED_OFF;
    setLEDState(false);
}

void LEDController::testPattern() {
    // Quick test pattern on startup
    setLEDState(true);
    delay(200);
    setLEDState(false);
    delay(100);
    setLEDState(true);
    delay(200);
    setLEDState(false);
    delay(100);
    setLEDState(true);
    delay(500);
    setLEDState(false);
}