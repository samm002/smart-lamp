#include <Arduino.h>
#include "WifiSetup.h"
#include "NTPSetup.h"
#define BAUD_RATE 115200

const char* ssid = "Zetta_House";
const char* password = "ZettaHouseBali2016";
String currentTimeStamp;

Wifi wifi(ssid, password);
TimeStamp timeStamp;

String currentDateStamp;
String timeStampNow;

void setup() {
  Serial.begin(BAUD_RATE);
  wifi.setup();
  timeStamp.setup();
}

void loop() {
  timeStamp.updateTime();
  timeStampNow = timeStamp.getTimeStamp();
  Serial.println(timeStampNow);
  delay(2000);
}
