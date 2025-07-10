#include "status_manager.h"

StatusManager statusManager;

portMUX_TYPE StatusManager::spinlock = portMUX_INITIALIZER_UNLOCKED;

StatusManager::StatusManager()
{
  systemReady = false;
}

void StatusManager::init()
{
  portENTER_CRITICAL(&spinlock);
  systemReady = false;
  portEXIT_CRITICAL(&spinlock);

  syncToThingsBoard();
  Serial.println("Status Manager initialized - System BUSY");
}

bool StatusManager::isReady() const
{
  portENTER_CRITICAL(&spinlock);
  bool ready = systemReady;
  portEXIT_CRITICAL(&spinlock);
  return ready;
}

bool StatusManager::isBusy() const
{
  return !isReady();
}

void StatusManager::setReady()
{
  setStatus(true);
}

void StatusManager::setBusy()
{
  setStatus(false);
}

void StatusManager::setStatus(bool ready)
{
  bool changed = false;

  portENTER_CRITICAL(&spinlock);
  if (systemReady != ready)
  {
    systemReady = ready;
    changed = true;
  }
  portEXIT_CRITICAL(&spinlock);

  if (changed)
  {
    syncToThingsBoard();
    Serial.print("System status changed to: ");
    Serial.println(ready ? "READY" : "BUSY");
  }
}

void StatusManager::syncToThingsBoard()
{
  // Note: ThingsBoard attributes must be sent from main file due to module separation
  // This will be handled by the main loop checking for status changes
}
