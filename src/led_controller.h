#ifndef LED_CONTROLLER_H
#define LED_CONTROLLER_H

#include <Arduino.h>
#include "constants.h"

// LED Pattern definitions
enum LedPattern
{
  LED_OFF,
  LED_SOLID_ON,
  LED_READY,               // Slow single blink every 2 seconds
  LED_POURING,             // Fast continuous toggle
  LED_POUR_COMPLETE,       // 3 medium blinks
  LED_SYSTEM_START,        // 1 long blink
  LED_SYSTEM_READY,        // 4 quick blinks
  LED_CUP_SIZE_CHANGE,     // 2 medium blinks
  LED_WIFI_CONNECT,        // 2 quick blinks
  LED_THINGSBOARD_CONNECT, // 2 quick blinks with pause
  LED_CONFIG_ERROR,        // Ultra fast red blinking - config validation failed
  LED_ERROR_CRITICAL,      // Very fast continuous blink
  LED_ERROR_WARNING,       // Medium speed blink
  LED_ERROR_INPUT,         // Short-long-short pattern
  LED_ERROR_TIMEOUT,       // Long-short-long pattern
  LED_ERROR_VOLUME,        // Short-short-long pattern
  LED_ERROR_NETWORK,       // Long-long-short pattern
  LED_ERROR_SENSOR         // Short-long-long pattern
};

struct LedState
{
  LedPattern currentPattern;
  LedPattern backgroundPattern;
  unsigned long lastUpdate;
  unsigned long patternStartTime;
  int stepIndex;
  bool ledPhysicalState;
  bool patternActive;
};

class LedController
{
private:
  LedState state;

public:
  LedController();
  void init();
  void setPattern(LedPattern pattern, bool isBackground = false);
  void update();
  void setLed(bool ledState);

  // Legacy function support for existing code
  void blinkRapidly(int duration, String reason);
  void blinkCount(int count, String reason);
};

// Global instance
extern LedController ledController;

#endif // LED_CONTROLLER_H
