#ifndef WIFI_PORTAL_H
#define WIFI_PORTAL_H

#include "config.h"
#include "PrinterState.h"
#include "WiFiCredentials.h"
#include "WiFiPortalHTML.h"

#ifdef ESP8266
  #include <ESP8266WebServer.h>
  #define WebServer ESP8266WebServer
#elif defined(ESP32)
  #include <WebServer.h>
#endif

// Forward declarations
extern WebServer server;
extern PrinterState printerState;
void updateLEDs();
void saveWiFiCredentials(const char* ssid, const char* password);

// Web server handlers
void handleRoot() {
  server.send(200, "text/html", getConfigPageHTML());
}

void handleSave() {
  if (server.hasArg("ssid") && server.hasArg("password")) {
    String ssid = server.arg("ssid");
    String password = server.arg("password");
    
    if (ssid.length() > 0) {
      saveWiFiCredentials(ssid.c_str(), password.c_str());
      storedSSID = ssid;
      storedPassword = password;
      wifiConfigured = true;
      
      server.send(200, "text/plain", "OK");
      Serial.println("WiFi credentials received and saved!");
      Serial.print("SSID: ");
      Serial.println(ssid);
      
      delay(1000);
      ESP.restart();
    } else {
      server.send(400, "text/plain", "SSID cannot be empty");
    }
  } else {
    server.send(400, "text/plain", "Missing parameters");
  }
}

void startWiFiPortal() {
  Serial.println("\n========================================");
  Serial.println("Starting WiFi Configuration Portal...");
  Serial.println("========================================");
  Serial.print("AP SSID: ");
  Serial.println(AP_SSID);
  Serial.print("AP Password: ");
  Serial.println(AP_PASSWORD);
  
  // Reset wifiConfigured flag to ensure portal stays open
  wifiConfigured = false;
  
  // Set LED to indicate AP mode
  printerState = Connecting;
  
  // Start AP mode
  WiFi.mode(WIFI_AP);
  bool apStarted = WiFi.softAP(AP_SSID, AP_PASSWORD);
  
  if (!apStarted) {
    Serial.println("ERROR: Failed to start AP mode!");
    delay(2000);
    return;
  }
  
  delay(100); // Give AP time to initialize
  
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
  
  // Setup web server routes
  server.on("/", handleRoot);
  server.on("/save", HTTP_POST, handleSave);
  server.begin();
  
  Serial.println("\n========================================");
  Serial.println("WiFi Portal is now active!");
  Serial.println("Connect to WiFi network:");
  Serial.print("  Name: ");
  Serial.println(AP_SSID);
  Serial.print("  Password: ");
  Serial.println(AP_PASSWORD);
  Serial.print("Then visit: http://");
  Serial.println(IP);
  Serial.println("========================================\n");
  
  // Keep portal running until WiFi is configured
  unsigned long portalStartTime = millis();
  unsigned long lastStatusPrint = 0;
  
  while (!wifiConfigured && (millis() - portalStartTime < 300000)) { // 5 minute timeout
    server.handleClient();
    updateLEDs();
    
    // Print status every 10 seconds
    if (millis() - lastStatusPrint > 10000) {
      Serial.print("Portal active - waiting for configuration... (");
      Serial.print((300000 - (millis() - portalStartTime)) / 1000);
      Serial.println(" seconds remaining)");
      lastStatusPrint = millis();
    }
    
    delay(10);
  }
  
  if (wifiConfigured) {
    Serial.println("\nWiFi credentials received! Closing portal...");
  } else {
    Serial.println("\nPortal timeout reached. Closing portal...");
  }
  
  server.stop();
  delay(100);
  WiFi.softAPdisconnect(true);
  Serial.println("WiFi portal closed");
}

void connectWiFi() {
  loadWiFiCredentials();
  
  if (!wifiConfigured || storedSSID.length() == 0) {
    startWiFiPortal();
    loadWiFiCredentials(); // Reload after portal
  }
  
  if (wifiConfigured && storedSSID.length() > 0) {
    Serial.print("Connecting to WiFi: ");
    Serial.println(storedSSID);
    WiFi.mode(WIFI_STA);
    WiFi.begin(storedSSID.c_str(), storedPassword.c_str());

    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 30) {
      delay(500);
      Serial.print(".");
      attempts++;
      updateLEDs();
    }

    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("\nWiFi connected!");
      Serial.print("IP Address: ");
      Serial.println(WiFi.localIP());
      Serial.println("WiFi connected — waiting for MQTT...");
    } else {
      Serial.println("\nWiFi connection failed! Restarting portal...");
      delay(2000);
      startWiFiPortal();
      ESP.restart();
    }
  }
}

#endif // WIFI_PORTAL_H
