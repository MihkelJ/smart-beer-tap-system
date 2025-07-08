#ifndef NETWORK_MANAGER_H
#define NETWORK_MANAGER_H

#include <Arduino.h>
#include <WiFi.h>
#include "constants.h"
#include "led_controller.h"

// Forward declarations for Blynk - actual includes will be in main file

class BlynkNetworkManager
{
private:
  bool lastWifiConnected;
  bool lastBlynkConnected;
  
public:
  BlynkNetworkManager();
  void init();
  void update();
  void initializeBlynkValues(float mlPerPulse);
  
  // Connection status
  bool isWifiConnected() const;
  bool isBlynkConnected() const;
  
  // Connection recovery
  void handleWifiStatusChange();
  void handleBlynkStatusChange(int currentCupSize, float mlPerPulse);
};

// Global instance
extern BlynkNetworkManager networkManager;

#endif // NETWORK_MANAGER_H