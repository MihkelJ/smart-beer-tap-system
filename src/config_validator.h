#ifndef CONFIG_VALIDATOR_H
#define CONFIG_VALIDATOR_H

#include <Arduino.h>

class ConfigValidator {
 private:
  bool hasValidConfig;
  String errorMessage;

 public:
  ConfigValidator();
  bool validateConfiguration();
  void displayConfigErrors();
  String getErrorMessage() const { return errorMessage; }
  bool isConfigValid() const { return hasValidConfig; }

 private:
  bool checkThingsBoardCredentials();
  bool checkWiFiCredentials();
  bool containsPlaceholder(const String &value);
  void setError(const String &error);
};

// Global instance
extern ConfigValidator configValidator;

#endif  // CONFIG_VALIDATOR_H
