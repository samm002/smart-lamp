#include <Arduino.h>
#include "WifiSetup.h"
#include "NTPSetup.h"
#include "Credentials.h"

#define BAUD_RATE 115200
#define LDR_SENSOR_ANALOG 39
#define LDR_SENSOR_DIGITAL 34
#define PIR_SENSOR 35
#define LED 33

Wifi wifi(ssid, password);
TimeStamp timeStamp;

bool ledState;
bool motionDetectedNotice;
bool pirState = false;

String currentTimeStamp;
String currentDateStamp;
String timeStampNow;

unsigned long currentTime;
unsigned long lastIdleTime = 0;
unsigned long idleInterval = 10000;

bool motionDetected();
bool lowLight();
bool idleTime();
void ledOn();
void ledOff();
void detectLight();
void detectMotion();

void setup() {
  Serial.begin(BAUD_RATE);
  wifi.setup();
  timeStamp.setup();
  pinMode(LDR_SENSOR_ANALOG, INPUT);
  pinMode(LDR_SENSOR_DIGITAL, INPUT);
  pinMode(PIR_SENSOR, INPUT);
  pinMode(LED, OUTPUT);
}

void loop() {
  timeStamp.updateTime();
  timeStampNow = timeStamp.getTimeStamp();
  detectLight();
}

void ledOn() {
  digitalWrite(LED, HIGH);
  ledState = true;
}

void ledOff() {
  digitalWrite(LED, LOW);
  ledState = false;
}

bool lowLight() {
  if (digitalRead(LDR_SENSOR_DIGITAL) == HIGH) {
    return true;
  } else {
    return false;
  }
}

bool motionDetected() {
  if (digitalRead(PIR_SENSOR) == HIGH) {
    return true;
  } else {
    return false;
  }
}

bool idleTime() {
  currentTime = millis();
  if (currentTime - lastIdleTime >= idleInterval)
    return true;
  else
    return false;
}

void detectMotion() {
  if (motionDetected()) {
    if (!motionDetectedNotice) {
      Serial.println("Motion Detected");
      Serial.print("Time Stamp : ");
      Serial.println(timeStampNow);
      ledOn();
      Serial.print("LED State : ");
      Serial.println(ledState);
      Serial.print("LDR State : ");
      Serial.println(digitalRead(LDR_SENSOR_DIGITAL));
      motionDetectedNotice = true;
      // send state led disini
    }
    lastIdleTime = currentTime;
  } else {
    if (idleTime() && motionDetectedNotice) {
      Serial.println("No Motion Detected for 10 Second");
      Serial.print("Time Stamp : ");
      Serial.println(timeStampNow);
      ledOff();
      Serial.print("LED State : ");
      Serial.println(ledState);
      Serial.print("LDR State : ");
      Serial.println(digitalRead(LDR_SENSOR_DIGITAL));
      motionDetectedNotice = false;
    }
  }
}

void detectLight() {
  if (lowLight()) {
    detectMotion();
    pirState = true;
  } else {
    if (idleTime() && pirState) {
      Serial.println("No Low Light & Motion Detected for 10 Second");
      Serial.print("Time Stamp : ");
      Serial.println(timeStampNow);
      ledOff();
      Serial.print("LED State : ");
      Serial.println(ledState);
      Serial.print("lowLight() : ");
      Serial.println(lowLight());
      Serial.print("LDR State : ");
      Serial.println(digitalRead(LDR_SENSOR_DIGITAL));
      lastIdleTime = currentTime;
      motionDetectedNotice = false;
      pirState = false;
      // send state led disini
    }
  }
}