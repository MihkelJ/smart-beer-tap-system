#ifndef STATUS_MANAGER_H
#define STATUS_MANAGER_H

#include <Arduino.h>
#include "constants.h"

class StatusManager
{
private:
  bool systemReady;
  static portMUX_TYPE spinlock;
  
public:
  StatusManager();
  void init();
  
  bool isReady() const;
  bool isBusy() const;
  
  void setReady();
  void setBusy();
  
private:
  void setStatus(bool ready);
  void syncToThingsBoard();
};

extern StatusManager statusManager;

#endif // STATUS_MANAGER_H