#include "WifiSetup.h"

Wifi::Wifi(const char* ssid, const char* password) :
  _ssid(ssid),
  _password(password) {}

void Wifi::setup() {
  previousWaitTime = 0;
  connectingInterval = 1000;
  currentMillis = 0;

  Serial.print("Connecting to wifi : ");
  Serial.println(_ssid);

  WiFi.begin(_ssid, _password);

  while (WiFi.status() != WL_CONNECTED && currentMillis - previousWaitTime < connectingInterval) {
    currentMillis = millis();
    if (currentMillis - previousWaitTime >= connectingInterval) {
      previousWaitTime = currentMillis;
      Serial.println("Connecting to wifi");
    }
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("Connected to wifi : ");
    Serial.println(_ssid);
    Serial.print("ESP32 Local IP Address : ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("Connection failed!");
  }
}
