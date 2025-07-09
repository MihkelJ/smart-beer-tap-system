#include "config_validator.h"
#include "config.h"

// Global instance
ConfigValidator configValidator;

ConfigValidator::ConfigValidator()
{
  hasValidConfig = false;
  errorMessage = "";
}

bool ConfigValidator::validateConfiguration()
{
  hasValidConfig = true;
  errorMessage = "";
  
  Serial.println("=== Configuration Validation ===");
  
  // Check ThingsBoard credentials
  if (!checkThingsBoardCredentials())
  {
    return false;
  }
  
  // Check WiFi credentials  
  if (!checkWiFiCredentials())
  {
    return false;
  }
  
  Serial.println("✅ Configuration validation passed!");
  return true;
}

bool ConfigValidator::checkThingsBoardCredentials()
{
  String server = String(THINGSBOARD_SERVER);
  String accessToken = String(THINGSBOARD_ACCESS_TOKEN);
  
  // Check for placeholder values
  if (containsPlaceholder(server))
  {
    setError("❌ THINGSBOARD_SERVER contains placeholder text");
    return false;
  }
  
  if (containsPlaceholder(accessToken))
  {
    setError("❌ THINGSBOARD_ACCESS_TOKEN contains placeholder text");
    return false;
  }
  
  // Check format and length
  if (server.length() < 5)
  {
    setError("❌ THINGSBOARD_SERVER too short (should be a valid URL)");
    return false;
  }
  
  if (accessToken.length() < 20)
  {
    setError("❌ THINGSBOARD_ACCESS_TOKEN too short (should be ~20 characters)");
    return false;
  }
  
  // Basic URL validation
  if (!server.endsWith(".io") && !server.endsWith(".com") && !server.endsWith(".org") && server.indexOf(".") < 0)
  {
    setError("❌ THINGSBOARD_SERVER format invalid (should be a valid domain)");
    return false;
  }
  
  Serial.println("✅ ThingsBoard credentials validated");
  return true;
}

bool ConfigValidator::checkWiFiCredentials()
{
  String ssid = String(WIFI_SSID);
  String password = String(WIFI_PASSWORD);
  
  // Check for placeholder values
  if (containsPlaceholder(ssid))
  {
    setError("❌ WIFI_SSID contains placeholder text");
    return false;
  }
  
  if (containsPlaceholder(password))
  {
    setError("❌ WIFI_PASSWORD contains placeholder text");
    return false;
  }
  
  // Check minimum lengths
  if (ssid.length() < 1)
  {
    setError("❌ WIFI_SSID is empty");
    return false;
  }
  
  if (password.length() < 8)
  {
    setError("❌ WIFI_PASSWORD too short (minimum 8 characters)");
    return false;
  }
  
  Serial.println("✅ WiFi credentials validated");
  return true;
}

bool ConfigValidator::containsPlaceholder(const String& value)
{
  // Check for common placeholder patterns
  return (value.indexOf("YOUR_") >= 0 || 
          value.indexOf("REPLACE") >= 0 ||
          value.indexOf("HERE") >= 0 ||
          value.indexOf("TEMPLATE") >= 0 ||
          value.indexOf("EXAMPLE") >= 0 ||
          value == "your" ||
          value == "example" ||
          value.length() == 0);
}

void ConfigValidator::setError(const String& error)
{
  hasValidConfig = false;
  errorMessage = error;
  Serial.println(error);
}

void ConfigValidator::displayConfigErrors()
{
  if (!hasValidConfig)
  {
    Serial.println("");
    Serial.println("🚨 CONFIGURATION ERROR 🚨");
    Serial.println(errorMessage);
    Serial.println("");
    Serial.println("📋 TO FIX:");
    Serial.println("1. Copy: examples/config_template.h → src/config.h");
    Serial.println("2. Edit src/config.h with your actual credentials");
    Serial.println("3. Upload code again");
    Serial.println("");
    Serial.println("📖 See SETUP.md for detailed instructions");
    Serial.println("");
    
    // Flash error pattern continuously
    ledController.setPattern(LED_ERROR_CRITICAL);
  }
}