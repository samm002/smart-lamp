#include <Arduino.h>
#include "WifiSetup.h"
#include "NTPSetup.h"
#include "Credentials.h"

#define BAUD_RATE 115200
#define LDR_SENSOR_ANALOG 34
#define LDR_SENSOR_DIGITAL 32
#define PIR_SENSOR 35

Wifi wifi(ssid, password);
TimeStamp timeStamp;

String currentTimeStamp;
String currentDateStamp;
String timeStampNow;

void setup() {
  Serial.begin(BAUD_RATE);
  wifi.setup();
  timeStamp.setup();
  pinMode(LDR_SENSOR_ANALOG, INPUT);
  pinMode(LDR_SENSOR_DIGITAL, INPUT);
  pinMode(PIR_SENSOR, INPUT);
}

void loop() {
  timeStamp.updateTime();
  timeStampNow = timeStamp.getTimeStamp();
  Serial.println(timeStampNow);
  Serial.println(digitalRead(PIR_SENSOR));
  Serial.println(digitalRead(LDR_SENSOR_DIGITAL));
  Serial.println(analogRead(LDR_SENSOR_ANALOG));
  delay(2000);
}
