#include "NTPSetup.h"

TimeStamp::TimeStamp() :
  _timezone(28800),
  timeClient(ntpUDP, "pool.ntp.org", _timezone) {}

void TimeStamp::setup() {
  timeClient.begin();
  timeClient.setTimeOffset(_timezone);
}

void TimeStamp::updateTime() {
  timeClient.update();
  _currentDateStamp = timeClient.getFormattedDate();
  _currentTimeStamp = timeClient.getFormattedTime();
  _timeStamp = _currentDateStamp + ", " + _currentTimeStamp;
}

int TimeStamp::getTimeZone() {
  return _timezone;
}

String TimeStamp::getCurrentDateStamp() {
  return _currentDateStamp;
}

String TimeStamp::getCurrentTimeStamp() {
  return _currentTimeStamp;
}

String TimeStamp::getTimeStamp() {
  return _timeStamp;
}

