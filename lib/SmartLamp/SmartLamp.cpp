#include "SmartLamp.h"

SmartLamp::SmartLamp() :
  _motionLastIdleTime(0),
  _lowLightIdleTime(0),
  _motionIdleInterval(10000),
  _lowLightIdleInterval(20000),
  timeStamp(), 
  wifi(ssid, password) {}

void SmartLamp::setup() {
  Serial.begin(BAUD_RATE);
  wifi.setup();
  timeStamp.setup();
  pinMode(LDR_SENSOR_ANALOG, INPUT);
  pinMode(LDR_SENSOR_DIGITAL, INPUT);
  pinMode(PIR_SENSOR, INPUT);
  pinMode(RELAY, OUTPUT);
}

void SmartLamp::lampOn() {
  digitalWrite(RELAY, HIGH);
  _relayState = true;
}

void SmartLamp::lampOff() {
  digitalWrite(RELAY, LOW);
  _relayState = false;
}

bool SmartLamp::lowLight() {
  if (digitalRead(LDR_SENSOR_DIGITAL) == HIGH) {
    return true;
  } else {
    return false;
  }
}

bool SmartLamp::motionDetected() {
  if (digitalRead(PIR_SENSOR) == HIGH) {
    return true;
  } else {
    return false;
  }
}

bool SmartLamp::motionIdleTime() {
  _currentTime = millis();
  if (_currentTime - _motionLastIdleTime >= _motionIdleInterval)
    return true;
  else
    return false;
}

bool SmartLamp::lowLightIdleTime() {
  _currentTime = millis();
  if (_currentTime - _lowLightIdleTime >= _lowLightIdleInterval)
    return true;
  else
    return false;
}

void SmartLamp::detectMotion() {
  _timeStampNow = timeStamp.getTimeStamp();
  if (motionDetected()) {
    if (!_motionDetectedNotice) {
      lampOn();
      Serial.println("Motion Detected");
      Serial.print("Time Stamp : ");
      Serial.println(_timeStampNow);
      _motionDetectedNotice = true;
      _pirState = true;
      // send state led disini
    }
    _motionLastIdleTime = _currentTime;
  } else {
    if (motionIdleTime() && _motionDetectedNotice) {
      lampOff();
      Serial.println("No Motion Detected for 10 Second");
      Serial.print("Time Stamp : ");
      Serial.println(_timeStampNow);
      _motionDetectedNotice = false;
    }
  }
}

void SmartLamp::detectLight() {
  _timeStampNow = timeStamp.getTimeStamp();
  timeStamp.updateTime();
  if (lowLight()) {
    detectMotion();
    _lowLightIdleTime = _currentTime;
  } else {
    if (lowLightIdleTime() && _pirState) {
      lampOff();
      Serial.println("No Low Light & Motion Detected for 20 Second");
      Serial.print("Time Stamp : ");
      Serial.println(_timeStampNow);
      _motionDetectedNotice = false;
      _pirState = false;
      // send state led disini
    }
  }
}