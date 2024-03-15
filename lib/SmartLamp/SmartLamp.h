#ifndef SMART_LAMP
#define SMART_LAMP

#include <Arduino.h>
#include "WifiSetup.h"
#include "NTPSetup.h"
#include "Credentials.h"

#define BAUD_RATE 115200
#define LDR_SENSOR_DIGITAL 39
#define LDR_SENSOR_ANALOG 34
#define PIR_SENSOR 35
#define LED 33

class SmartLamp {
  private:
    bool _ledState;
    bool _motionDetectedNotice;
    bool _pirState;

    String _currentTimeStamp;
    String _currentDateStamp;
    String _timeStampNow;

    unsigned long _currentTime;
    unsigned long _motionLastIdleTime;
    unsigned long _lowLightIdleTime;
    unsigned long _motionIdleInterval;
    unsigned long _lowLightIdleInterval;

    Wifi wifi;
    TimeStamp timeStamp;

  public:
    SmartLamp();
    void setup();

    bool motionDetected();
    bool lowLight();
    bool motionIdleTime();
    bool lowLightIdleTime();
    
    void ledOn();
    void ledOff();
    void detectLight();
    void detectMotion();
};

#endif