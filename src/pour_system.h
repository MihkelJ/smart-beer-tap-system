#ifndef POUR_SYSTEM_H
#define POUR_SYSTEM_H

#include <Arduino.h>
#include <WiFi.h>
#include "constants.h"

class PourSystem {
 private:
  // Flow sensor variables
  volatile unsigned long pulseCount;
  float mlPerPulse;
  static portMUX_TYPE spinlock;

  // Pour tracking variables
  float totalVolume;
  unsigned long pourStartTime;

  // System state flags
  bool isPouring;
  int currentCupSize;

  // Timing variables
  unsigned long lastWatchdogTime;

 public:
  PourSystem();
  void init();
  void update();
  void checkWatchdog();

  // Pour control
  void startPour();
  void stopPour();
  void emergencyStop();
  void resetCounters();

  // Relay control
  void setRelay(bool state);

  // ThingsBoard RPC handlers
  void handleCupSizeChange(int value);
  void handleMlPerPulseChange(float value);

  // Flow sensor
  static void IRAM_ATTR pulseCounter();

  // Getters for status
  bool getIsReady() const { return !isPouring; }
  bool getIsPouring() const { return isPouring; }
  float getTotalVolume() const { return totalVolume; }
  int getCurrentCupSize() const { return currentCupSize; }
  float getMlPerPulse() const { return mlPerPulse; }

  // Safety checks
  bool performSafetyChecks(bool wifiConnected, bool thingsBoardConnected);
};

// Global instance
extern PourSystem pourSystem;

#endif  // POUR_SYSTEM_H
