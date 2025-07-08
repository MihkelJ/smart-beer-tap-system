#ifndef POUR_SYSTEM_H
#define POUR_SYSTEM_H

#include <Arduino.h>
#include <WiFi.h>
#include "constants.h"
#include "led_controller.h"

class PourSystem
{
private:
  // Flow sensor variables
  volatile unsigned long pulseCount;
  float mlPerPulse;
  static portMUX_TYPE spinlock;
  
  // Pour tracking variables
  float totalVolume;
  unsigned long pourStartTime;
  unsigned long totalPours;
  float totalVolumePoured;
  
  // System state flags
  bool isReady;
  bool isPouring;
  int currentCupSize;
  String lastStatus;
  
  // Timing variables
  unsigned long lastWatchdogTime;
  unsigned long lastStatsTime;
  static unsigned long lastPulseTime;
  
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
  
  // Status management
  void updateStatus(const String &status);
  
  // Blynk handlers
  void handleCupSizeChange(int value);
  void handleMlPerPulseChange(float value);
  
  // Flow sensor
  static void IRAM_ATTR pulseCounter();
  
  // Getters for status
  bool getIsReady() const { return isReady; }
  bool getIsPouring() const { return isPouring; }
  float getTotalVolume() const { return totalVolume; }
  int getCurrentCupSize() const { return currentCupSize; }
  float getMlPerPulse() const { return mlPerPulse; }
  
  // Safety checks
  bool performSafetyChecks(bool wifiConnected, bool blynkConnected);
  void checkSensorHealth();
};

// Global instance
extern PourSystem pourSystem;

#endif // POUR_SYSTEM_H