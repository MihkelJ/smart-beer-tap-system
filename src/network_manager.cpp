#include "network_manager.h"

// ThingsBoard functions will be called from main file

// Global instance
ThingsBoardNetworkManager networkManager;

ThingsBoardNetworkManager::ThingsBoardNetworkManager() { lastWifiConnected = false; }

void ThingsBoardNetworkManager::init() {
  Serial.println("Status: Connecting to WiFi");
  // ThingsBoard.connect will be called from main file
}

void ThingsBoardNetworkManager::handleWifiStatusChange() {
  bool wifiConnected = (WiFi.status() == WL_CONNECTED);
  if (wifiConnected != lastWifiConnected) {
    if (wifiConnected) {
      Serial.println("WiFi connection established");
    } else {
      Serial.println("WiFi connection failed - attempting reconnect");
      WiFi.reconnect();
    }
    lastWifiConnected = wifiConnected;
  }
}
