#ifndef NETWORK_MANAGER_H
#define NETWORK_MANAGER_H

#include <Arduino.h>
#include <WiFi.h>
#include "constants.h"

// Forward declarations for ThingsBoard - actual includes will be in main file

class ThingsBoardNetworkManager
{
private:
  bool lastWifiConnected;

public:
  ThingsBoardNetworkManager();
  void init();
  // Connection recovery
  void handleWifiStatusChange();
};

// Global instance
extern ThingsBoardNetworkManager networkManager;

#endif // NETWORK_MANAGER_H
