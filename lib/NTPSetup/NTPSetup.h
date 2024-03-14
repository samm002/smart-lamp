#ifndef NTP_SETUP
#define NTP_SETUP

#include <WiFiUdp.h>
#include "NTPClient.h"

class TimeStamp
{
  private:
    int _timezone;

    String _currentDateStamp;
    String _currentTimeStamp;
    String _timeStamp;

    WiFiUDP ntpUDP;
    NTPClient timeClient;

  public:
    TimeStamp();
    void begin();
    void setup();
    void updateTime();

    int getTimeZone();
    String getCurrentTimeStamp();
    String getCurrentDateStamp();
    String getTimeStamp();
};

#endif