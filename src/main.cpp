#include <Arduino.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include "NTPClient.h"
#define BAUD_RATE 115200

const char* ssid = "Zetta_House";
const char* password = "ZettaHouseBali2016";
String currentTimeStamp;
const unsigned int minute = 60;
const unsigned int hour = 60 * minute;
const unsigned int wita = 8 * hour;

String currentDateStamp;
String timeStamp;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", wita);

void wifiSetup(void);
void ntpSetup(void);
void updateTime(void);

void setup() {
  Serial.begin(BAUD_RATE);
  wifiSetup();
  ntpSetup();
}

void loop() {
  updateTime();
  Serial.println(timeStamp);
  delay(2000);
}

void wifiSetup(void) {
  Serial.print("Connecting to wifi : ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.print("Connected to wifi : ");
  Serial.println(ssid);
  Serial.print("ESP 32 Local IP Address : ");
  Serial.println(WiFi.localIP());
}

void ntpSetup(void) {
  timeClient.begin();
  timeClient.setTimeOffset(wita);
}

void updateTime(void) {
  timeClient.update();
  currentDateStamp = timeClient.getFormattedDate();
  currentTimeStamp = timeClient.getFormattedTime();
  timeStamp = currentDateStamp + ", " + currentTimeStamp;
}