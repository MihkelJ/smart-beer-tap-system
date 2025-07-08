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
  
  // Check Blynk credentials
  if (!checkBlynkCredentials())
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

bool ConfigValidator::checkBlynkCredentials()
{
  String templateId = String(BLYNK_TEMPLATE_ID);
  String templateName = String(BLYNK_TEMPLATE_NAME);
  String authToken = String(BLYNK_AUTH_TOKEN);
  
  // Check for placeholder values
  if (containsPlaceholder(templateId))
  {
    setError("❌ BLYNK_TEMPLATE_ID contains placeholder text");
    return false;
  }
  
  if (containsPlaceholder(templateName))
  {
    setError("❌ BLYNK_TEMPLATE_NAME contains placeholder text");
    return false;
  }
  
  if (containsPlaceholder(authToken))
  {
    setError("❌ BLYNK_AUTH_TOKEN contains placeholder text");
    return false;
  }
  
  // Check format and length
  if (templateId.length() < 5 || !templateId.startsWith("TMPL"))
  {
    setError("❌ BLYNK_TEMPLATE_ID format invalid (should start with TMPL)");
    return false;
  }
  
  if (templateName.length() < 3)
  {
    setError("❌ BLYNK_TEMPLATE_NAME too short");
    return false;
  }
  
  if (authToken.length() < 20)
  {
    setError("❌ BLYNK_AUTH_TOKEN too short (should be ~32 characters)");
    return false;
  }
  
  Serial.println("✅ Blynk credentials validated");
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