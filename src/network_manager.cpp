#include "network_manager.h"

// ThingsBoard functions will be called from main file

// Global instance
ThingsBoardNetworkManager networkManager;

ThingsBoardNetworkManager::ThingsBoardNetworkManager()
{
  lastWifiConnected = false;
  lastThingsBoardConnected = false;
}

void ThingsBoardNetworkManager::init()
{
  // Show connecting pattern
  ledController.setPattern(LED_ERROR_WARNING); // Use warning pattern for connecting
  Serial.println("Status: Connecting to WiFi");
  // ThingsBoard.connect will be called from main file
}

void ThingsBoardNetworkManager::initializeThingsBoardValues(float mlPerPulse)
{
  // ThingsBoard initialization will be called from main file
  Serial.println("Initializing ThingsBoard values");
}

bool ThingsBoardNetworkManager::isWifiConnected() const
{
  return (WiFi.status() == WL_CONNECTED);
}

bool ThingsBoardNetworkManager::isThingsBoardConnected() const
{
  // ThingsBoard.connected() will be checked from main file
  return true; // Placeholder - will be managed by main file
}

void ThingsBoardNetworkManager::handleWifiStatusChange()
{
  bool wifiConnected = isWifiConnected();
  if (wifiConnected != lastWifiConnected)
  {
    if (wifiConnected)
    {
      ledController.blinkCount(2, "WiFi connection established");
    }
    else
    {
      ledController.blinkRapidly(1000, "WiFi connection failed");
      WiFi.reconnect();
    }
    lastWifiConnected = wifiConnected;
  }
}

void ThingsBoardNetworkManager::handleThingsBoardStatusChange(int currentCupSize, float mlPerPulse)
{
  // ThingsBoard status handling removed - no more spam messages
}

void ThingsBoardNetworkManager::update()
{
  // ThingsBoard.loop() will be called from main file
}
