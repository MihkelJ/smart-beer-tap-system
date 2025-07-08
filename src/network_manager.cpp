#include "network_manager.h"

// Blynk functions will be called from main file

// Global instance
BlynkNetworkManager networkManager;

BlynkNetworkManager::BlynkNetworkManager()
{
  lastWifiConnected = false;
  lastBlynkConnected = false;
}

void BlynkNetworkManager::init()
{
  // Show connecting pattern
  ledController.setPattern(LED_ERROR_WARNING); // Use warning pattern for connecting
  Serial.println("Status: Connecting to WiFi");
  // Blynk.begin will be called from main file
}

void BlynkNetworkManager::initializeBlynkValues(float mlPerPulse)
{
  // Blynk initialization will be called from main file
  Serial.println("Initializing Blynk values");
}

bool BlynkNetworkManager::isWifiConnected() const
{
  return (WiFi.status() == WL_CONNECTED);
}

bool BlynkNetworkManager::isBlynkConnected() const
{
  // Blynk.connected() will be checked from main file
  return true; // Placeholder - will be managed by main file
}

void BlynkNetworkManager::handleWifiStatusChange()
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

void BlynkNetworkManager::handleBlynkStatusChange(int currentCupSize, float mlPerPulse)
{
  // Blynk status handling removed - no more spam messages
}

void BlynkNetworkManager::update()
{
  // Blynk.run() will be called from main file
}