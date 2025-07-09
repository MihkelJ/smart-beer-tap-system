#include "led_controller.h"

// Global instance
LedController ledController;

LedController::LedController()
{
  state = {LED_OFF, LED_OFF, 0, 0, 0, false, false};
}

void LedController::init()
{
  pinMode(LED_PIN, OUTPUT);
  setLed(false);
}

void LedController::setLed(bool ledState)
{
  digitalWrite(LED_PIN, ledState);
  state.ledPhysicalState = ledState;
}

void LedController::setPattern(LedPattern pattern, bool isBackground)
{
  if (isBackground)
  {
    state.backgroundPattern = pattern;
  }
  else
  {
    state.currentPattern = pattern;
    state.patternStartTime = millis();
    state.stepIndex = 0;
    state.patternActive = true;
  }
}

void LedController::update()
{
  unsigned long currentTime = millis();
  LedPattern activePattern = state.currentPattern != LED_OFF ? state.currentPattern : state.backgroundPattern;

  if (activePattern == LED_OFF)
  {
    setLed(false);
    return;
  }

  // Pattern timing definitions
  switch (activePattern)
  {
  case LED_SOLID_ON:
    setLed(true);
    break;

  case LED_READY:
    if (currentTime - state.lastUpdate > 2000)
    {
      setLed(!state.ledPhysicalState);
      state.lastUpdate = currentTime;
    }
    break;

  case LED_POURING:
    if (currentTime - state.lastUpdate > 250)
    {
      setLed(!state.ledPhysicalState);
      state.lastUpdate = currentTime;
    }
    break;

  case LED_POUR_COMPLETE:
  {
    int intervals[] = {200, 200, 200, 200, 200, 200, 1000}; // 3 blinks + pause
    if (state.stepIndex < 7)
    {
      if (currentTime - state.lastUpdate > intervals[state.stepIndex])
      {
        setLed(state.stepIndex % 2 == 0);
        state.lastUpdate = currentTime;
        state.stepIndex++;
      }
    }
    else
    {
      state.currentPattern = LED_OFF;
    }
  }
  break;

  case LED_SYSTEM_START:
    if (currentTime - state.patternStartTime < 500)
    {
      setLed(true);
    }
    else if (currentTime - state.patternStartTime < 1000)
    {
      setLed(false);
    }
    else
    {
      state.currentPattern = LED_OFF;
    }
    break;

  case LED_SYSTEM_READY:
  {
    int intervals[] = {150, 150, 150, 150, 150, 150, 150, 150, 1000}; // 4 quick blinks
    if (state.stepIndex < 9)
    {
      if (currentTime - state.lastUpdate > intervals[state.stepIndex])
      {
        setLed(state.stepIndex % 2 == 0);
        state.lastUpdate = currentTime;
        state.stepIndex++;
      }
    }
    else
    {
      state.currentPattern = LED_OFF;
    }
  }
  break;

  case LED_CUP_SIZE_CHANGE:
  {
    int intervals[] = {200, 200, 200, 200, 1000}; // 2 medium blinks
    if (state.stepIndex < 5)
    {
      if (currentTime - state.lastUpdate > intervals[state.stepIndex])
      {
        setLed(state.stepIndex % 2 == 0);
        state.lastUpdate = currentTime;
        state.stepIndex++;
      }
    }
    else
    {
      state.currentPattern = LED_OFF;
    }
  }
  break;

  case LED_WIFI_CONNECT:
  {
    int intervals[] = {150, 150, 150, 150, 1000}; // 2 quick blinks
    if (state.stepIndex < 5)
    {
      if (currentTime - state.lastUpdate > intervals[state.stepIndex])
      {
        setLed(state.stepIndex % 2 == 0);
        state.lastUpdate = currentTime;
        state.stepIndex++;
      }
    }
    else
    {
      state.currentPattern = LED_OFF;
    }
  }
  break;

  case LED_THINGSBOARD_CONNECT:
  {
    int intervals[] = {150, 150, 300, 150, 150, 1000}; // 2 quick blinks with pause
    if (state.stepIndex < 6)
    {
      if (currentTime - state.lastUpdate > intervals[state.stepIndex])
      {
        setLed(state.stepIndex % 2 == 0 && state.stepIndex != 2);
        state.lastUpdate = currentTime;
        state.stepIndex++;
      }
    }
    else
    {
      state.currentPattern = LED_OFF;
    }
  }
  break;

  case LED_ERROR_CRITICAL:
    if (currentTime - state.lastUpdate > 100)
    {
      setLed(!state.ledPhysicalState);
      state.lastUpdate = currentTime;
    }
    break;

  case LED_ERROR_WARNING:
    if (currentTime - state.lastUpdate > 300)
    {
      setLed(!state.ledPhysicalState);
      state.lastUpdate = currentTime;
    }
    break;

  case LED_ERROR_INPUT:
  {
    int intervals[] = {100, 100, 300, 100, 100, 100, 1000}; // Short-long-short + pause
    if (state.stepIndex < 7)
    {
      if (currentTime - state.lastUpdate > intervals[state.stepIndex])
      {
        setLed(state.stepIndex % 2 == 0);
        state.lastUpdate = currentTime;
        state.stepIndex++;
      }
    }
    else
    {
      state.stepIndex = 0;
      state.lastUpdate = currentTime;
    }
  }
  break;

  case LED_ERROR_TIMEOUT:
  {
    int intervals[] = {300, 100, 100, 100, 300, 100, 1000}; // Long-short-long + pause
    if (state.stepIndex < 7)
    {
      if (currentTime - state.lastUpdate > intervals[state.stepIndex])
      {
        setLed(state.stepIndex % 2 == 0);
        state.lastUpdate = currentTime;
        state.stepIndex++;
      }
    }
    else
    {
      state.stepIndex = 0;
      state.lastUpdate = currentTime;
    }
  }
  break;

  case LED_ERROR_VOLUME:
  {
    int intervals[] = {100, 100, 100, 100, 300, 100, 1000}; // Short-short-long + pause
    if (state.stepIndex < 7)
    {
      if (currentTime - state.lastUpdate > intervals[state.stepIndex])
      {
        setLed(state.stepIndex % 2 == 0);
        state.lastUpdate = currentTime;
        state.stepIndex++;
      }
    }
    else
    {
      state.stepIndex = 0;
      state.lastUpdate = currentTime;
    }
  }
  break;

  case LED_ERROR_NETWORK:
  {
    int intervals[] = {300, 100, 300, 100, 100, 100, 1000}; // Long-long-short + pause
    if (state.stepIndex < 7)
    {
      if (currentTime - state.lastUpdate > intervals[state.stepIndex])
      {
        setLed(state.stepIndex % 2 == 0);
        state.lastUpdate = currentTime;
        state.stepIndex++;
      }
    }
    else
    {
      state.stepIndex = 0;
      state.lastUpdate = currentTime;
    }
  }
  break;

  case LED_ERROR_SENSOR:
  {
    int intervals[] = {100, 100, 300, 100, 300, 100, 1000}; // Short-long-long + pause
    if (state.stepIndex < 7)
    {
      if (currentTime - state.lastUpdate > intervals[state.stepIndex])
      {
        setLed(state.stepIndex % 2 == 0);
        state.lastUpdate = currentTime;
        state.stepIndex++;
      }
    }
    else
    {
      state.stepIndex = 0;
      state.lastUpdate = currentTime;
    }
  }
  break;

  case LED_CONFIG_ERROR:
    // Ultra fast blinking for config errors
    if (currentTime - state.lastUpdate > 50)
    {
      setLed(!state.ledPhysicalState);
      state.lastUpdate = currentTime;
    }
    break;

  case LED_SETUP_MODE:
  {
    // Breathing pattern using sine wave
    unsigned long breathCycle = (currentTime / 10) % 628; // 0-628 (~2*PI*100)
    float intensity = (sin(breathCycle / 100.0) + 1.0) / 2.0; // 0.0 to 1.0
    
    // Simple PWM-like effect by varying blink rate
    int blinkInterval = 20 + (int)(80 * intensity); // 20ms to 100ms
    if (currentTime - state.lastUpdate > blinkInterval)
    {
      setLed(!state.ledPhysicalState);
      state.lastUpdate = currentTime;
    }
  }
  break;

  default:
    setLed(false);
    break;
  }
}

void LedController::blinkRapidly(int duration, String reason)
{
  Serial.println("Status: " + reason);
  // Use appropriate error pattern based on reason
  if (reason.indexOf("timeout") >= 0)
  {
    setPattern(LED_ERROR_TIMEOUT);
  }
  else if (reason.indexOf("volume") >= 0)
  {
    setPattern(LED_ERROR_VOLUME);
  }
  else if (reason.indexOf("WiFi") >= 0 || reason.indexOf("ThingsBoard") >= 0)
  {
    setPattern(LED_ERROR_NETWORK);
  }
  else if (reason.indexOf("Invalid") >= 0)
  {
    setPattern(LED_ERROR_INPUT);
  }
  else if (reason.indexOf("sensor") >= 0)
  {
    setPattern(LED_ERROR_SENSOR);
  }
  else
  {
    setPattern(LED_ERROR_CRITICAL);
  }
}

void LedController::blinkCount(int count, String reason)
{
  Serial.println("Status: " + reason);
  // Use appropriate pattern based on count and reason
  if (count == 1)
  {
    setPattern(LED_SYSTEM_START);
  }
  else if (count == 2)
  {
    if (reason.indexOf("WiFi") >= 0)
    {
      setPattern(LED_WIFI_CONNECT);
    }
    else if (reason.indexOf("ThingsBoard") >= 0)
    {
      setPattern(LED_THINGSBOARD_CONNECT);
    }
    else
    {
      setPattern(LED_CUP_SIZE_CHANGE);
    }
  }
  else if (count == 3)
  {
    setPattern(LED_POUR_COMPLETE);
  }
  else if (count == 4)
  {
    setPattern(LED_SYSTEM_READY);
  }
}