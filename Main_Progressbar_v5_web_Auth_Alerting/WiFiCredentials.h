#ifndef WIFI_CREDENTIALS_H
#define WIFI_CREDENTIALS_H

#include "config.h"

#ifdef ESP8266
  #include <EEPROM.h>
  #define EEPROM_SIZE 512
  #define MAGIC_BYTE_ADDR 0
  #define MAGIC_BYTE_VALUE 0xAA
  #define SSID_ADDR 1
  #define PASS_ADDR 65
#elif defined(ESP32)
  #include <Preferences.h>
  extern Preferences preferences;
#endif

// Global variables for stored credentials
extern bool wifiConfigured;
extern String storedSSID;
extern String storedPassword;

// Function declarations
void saveWiFiCredentials(const char* ssid, const char* password);
void loadWiFiCredentials();

// Helper function to validate string (only printable ASCII, reasonable length)
bool isValidString(const String& str, int maxLen) {
  if (str.length() == 0 || str.length() > maxLen) return false;
  for (unsigned int i = 0; i < str.length(); i++) {
    char c = str.charAt(i);
    // Allow printable ASCII (32-126) and some common special chars
    if (c < 32 || c > 126) {
      if (c != 0) return false; // Only allow null terminator
    }
  }
  return true;
}

// Function implementations
void saveWiFiCredentials(const char* ssid, const char* password) {
#ifdef ESP8266
  EEPROM.begin(EEPROM_SIZE);
  // Write magic byte to indicate valid data
  EEPROM.write(MAGIC_BYTE_ADDR, MAGIC_BYTE_VALUE);
  // Write SSID
  int ssidLen = strlen(ssid);
  for (int i = 0; i < 64; i++) {
    EEPROM.write(SSID_ADDR + i, (i < ssidLen) ? ssid[i] : 0);
  }
  // Write Password
  int passLen = strlen(password);
  for (int i = 0; i < 64; i++) {
    EEPROM.write(PASS_ADDR + i, (i < passLen) ? password[i] : 0);
  }
  EEPROM.commit();
  EEPROM.end();
  Serial.println("WiFi credentials saved to EEPROM!");
#elif defined(ESP32)
  preferences.begin("wifi", false);
  preferences.putString("ssid", ssid);
  preferences.putString("pass", password);
  preferences.putBool("valid", true);
  preferences.end();
  Serial.println("WiFi credentials saved to Preferences!");
#endif
  Serial.print("SSID: ");
  Serial.println(ssid);
}

void loadWiFiCredentials() {
  storedSSID = "";
  storedPassword = "";
  wifiConfigured = false;

#ifdef ESP8266
  EEPROM.begin(EEPROM_SIZE);
  
  // Check magic byte to verify data is valid
  uint8_t magic = EEPROM.read(MAGIC_BYTE_ADDR);
  if (magic != MAGIC_BYTE_VALUE) {
    EEPROM.end();
    Serial.println("EEPROM not initialized or corrupted - will start AP mode");
    return;
  }
  
  // Read SSID
  for (int i = 0; i < 64; i++) {
    char c = EEPROM.read(SSID_ADDR + i);
    if (c == 0) break;
    storedSSID += c;
  }
  
  // Read Password
  for (int i = 0; i < 64; i++) {
    char c = EEPROM.read(PASS_ADDR + i);
    if (c == 0) break;
    storedPassword += c;
  }
  EEPROM.end();
  
  // Validate the read data
  if (!isValidString(storedSSID, 64)) {
    Serial.println("Invalid SSID in EEPROM - will start AP mode");
    storedSSID = "";
    storedPassword = "";
    return;
  }
  
#elif defined(ESP32)
  preferences.begin("wifi", true);
  bool valid = preferences.getBool("valid", false);
  if (!valid) {
    preferences.end();
    Serial.println("Preferences not initialized - will start AP mode");
    return;
  }
  storedSSID = preferences.getString("ssid", "");
  storedPassword = preferences.getString("pass", "");
  preferences.end();
  
  // Validate the read data
  if (!isValidString(storedSSID, 64)) {
    Serial.println("Invalid SSID in Preferences - will start AP mode");
    storedSSID = "";
    storedPassword = "";
    return;
  }
#endif

  // Use defaults if stored credentials are empty but defaults exist
  if (storedSSID.length() == 0 && strlen(DEFAULT_WIFI_SSID) > 0) {
    storedSSID = String(DEFAULT_WIFI_SSID);
    storedPassword = String(DEFAULT_WIFI_PASS);
    Serial.println("Using default WiFi credentials from config.h");
  }

  if (storedSSID.length() > 0) {
    wifiConfigured = true;
    Serial.print("Loaded WiFi SSID: ");
    Serial.println(storedSSID);
  } else {
    wifiConfigured = false;
    Serial.println("No WiFi credentials found - will start AP mode");
  }
}

#endif // WIFI_CREDENTIALS_H
