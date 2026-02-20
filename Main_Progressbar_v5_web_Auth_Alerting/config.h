#ifndef CONFIG_H
#define CONFIG_H

// ===============================
// USER CONFIGURATION
// ===============================
// WiFi credentials - can be set via web portal or defaults below
// If empty strings, device will start in AP mode for configuration
const char* DEFAULT_WIFI_SSID = "";  // Leave empty to use web portal
const char* DEFAULT_WIFI_PASS = "";  // Leave empty to use web portal

// ===============================
// USER CONFIGURATION
// ===============================
const char* WIFI_SSID = "Get-7368B0";
const char* WIFI_PASS = "qjzkld2mjy";

const char* MQTT_SERVER = "192.168.0.49";
const int MQTT_PORT = 8883;
const char* MQTT_USER = "bblp";
const char* MQTT_PASS = "10329878";
const char* PRINTER_SERIAL = "01P199570600151";  // Printer Serial

// ===============================
// LED CONFIGURATION
// ===============================
#define DATA_PIN 4
#define NUM_LEDS 13
#define BRIGHTNESS 64
#define MAX_MILLIWATTS 850

// WiFi Portal Configuration
const char* AP_SSID = "StatusBarPro-Setup";
const char* AP_PASSWORD = "setup12345";  // Password for AP mode

// ===============================
// SOUND CONFIGURATION
// ===============================
#define BUZZER_PIN 6        // Digital pin for buzzer (change if needed)
#define SOUND_ENABLED true  // Set to false to disable sound alerts

#endif // CONFIG_H
