#include "NTPClient.h"

NTPClient::NTPClient(UDP& udp) {
  this->_udp            = &udp;
}

NTPClient::NTPClient(UDP& udp, int timeOffset) {
  this->_udp            = &udp;
  this->_timeOffset     = timeOffset;
}

NTPClient::NTPClient(UDP& udp, const char* poolServerName) {
  this->_udp            = &udp;
  this->_poolServerName = poolServerName;
}

NTPClient::NTPClient(UDP& udp, const char* poolServerName, int timeOffset) {
  this->_udp            = &udp;
  this->_timeOffset     = timeOffset;
  this->_poolServerName = poolServerName;
}

NTPClient::NTPClient(UDP& udp, const char* poolServerName, int timeOffset, unsigned long updateInterval) {
  this->_udp            = &udp;
  this->_timeOffset     = timeOffset;
  this->_poolServerName = poolServerName;
  this->_updateInterval = updateInterval;
}

void NTPClient::begin() {
  this->begin(NTP_DEFAULT_LOCAL_PORT);
}

void NTPClient::begin(int port) {
  this->_port = port;

  this->_udp->begin(this->_port);

  this->_udpSetup = true;
}

bool NTPClient::isValid(byte * ntpPacket)
{
  //Perform a few validity checks on the packet
  if((ntpPacket[0] & 0b11000000) == 0b11000000)   //Check for LI=UNSYNC
    return false;
    
  if((ntpPacket[0] & 0b00111000) >> 3 < 0b100)    //Check for Version >= 4
    return false;
    
  if((ntpPacket[0] & 0b00000111) != 0b100)      //Check for Mode == Server
    return false;
    
  if((ntpPacket[1] < 1) || (ntpPacket[1] > 15))   //Check for valid Stratum
    return false;

  if( ntpPacket[16] == 0 && ntpPacket[17] == 0 && 
    ntpPacket[18] == 0 && ntpPacket[19] == 0 &&
    ntpPacket[20] == 0 && ntpPacket[21] == 0 &&
    ntpPacket[22] == 0 && ntpPacket[22] == 0)   //Check for ReferenceTimestamp != 0
    return false;

  return true;
}

bool NTPClient::forceUpdate() {
  #ifdef DEBUG_NTPClient
    Serial.println("Update from NTP Server");
  #endif
  // flush any existing packets
  while(this->_udp->parsePacket() != 0)
    this->_udp->flush();
  this->sendNTPPacket();

  // Wait till data is there or timeout...
  byte timeout = 0;
  int cb = 0;
  do {
    delay ( 10 );
    cb = this->_udp->parsePacket();
    
    if(cb > 0)
    {
      this->_udp->read(this->_packetBuffer, NTP_PACKET_SIZE);
      if(!this->isValid(this->_packetBuffer))
        cb = 0;
    }
    
    if (timeout > 100) return false; // timeout after 1000 ms
    timeout++;
  } while (cb == 0);

  this->_lastUpdate = millis() - (10 * (timeout + 1)); // Account for delay in reading the time

  unsigned long highWord = word(this->_packetBuffer[40], this->_packetBuffer[41]);
  unsigned long lowWord = word(this->_packetBuffer[42], this->_packetBuffer[43]);
  // combine the four bytes (two words) into a long integer
  // this is NTP time (seconds since Jan 1 1900):
  unsigned long secsSince1900 = highWord << 16 | lowWord;

  this->_currentEpoc = secsSince1900 - SEVENZYYEARS;

  return true;
}

bool NTPClient::update() {
  if ((millis() - this->_lastUpdate >= this->_updateInterval)     // Update after _updateInterval
    || this->_lastUpdate == 0) {                                // Update if there was no update yet.
    if (!this->_udpSetup) this->begin();                         // setup the UDP client if needed
    return this->forceUpdate();
  }
  return true;
}

unsigned long NTPClient::getEpochTime() {
  return this->_timeOffset + // User offset
         this->_currentEpoc + // Epoc returned by the NTP server
         ((millis() - this->_lastUpdate) / 1000); // Time since last update
}

int NTPClient::getYear() {
  const unsigned long SECONDS_IN_YEAR = 365 * 24 * 60 * 60;
  return (this->getEpochTime() / SECONDS_IN_YEAR) + 1970; // Add 1970 to get the actual year
}

int NTPClient::getMonth() {
  const unsigned long SECONDS_IN_MONTH = 30 * 24 * 60 * 60;
  return ((((this->getEpochTime()  / SECONDS_IN_MONTH) + 4 ) % 12)); // 1 is January
}

String NTPClient::getMonthName() {
    unsigned long month = getMonth();
    static const char* monthNames[] = {
        "January", "February", "March", "April", "May", "June",
        "July", "August", "September", "October", "November", "December"
    };

    if (month > 0 && month <= 12) {
        return monthNames[month - 1];
    }

    return "Invalid Month";
}

int NTPClient::getDate() {
  unsigned long rawTime = this->getEpochTime() / 86400L;
  unsigned long days = 0;
  uint8_t month;
  int year = getYear();
  uint8_t monthOfYear = getMonth();

  while((days += (LEAP_YEAR(year) ? 366 : 365)) <= rawTime)
    year++;

  static const uint8_t monthDays[] = {31, LEAP_YEAR(year) ? 29 : 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

  rawTime -= days - (LEAP_YEAR(year) ? 366 : 365);

  for (month = 0; month < monthOfYear; month++) {
  uint8_t monthLength;
    monthLength = monthDays[month];
    if (rawTime < monthLength) break;
    rawTime -= monthLength;
  }

  return rawTime + 1;
}

int NTPClient::getDay() {
  return (((this->getEpochTime()  / 86400L) + 4 ) % 7); // 0 is Sunday
}

String NTPClient::getDayName() {
    unsigned long day = getDay();
    static const char* dayNames[] = {
        "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"
    };

    if (day >= 0 && day <= 6) {
        return dayNames[day];
    }

    return "Saturday";
}

int NTPClient::getHour() {
  return ((this->getEpochTime()  % 86400L) / 3600);
}

int NTPClient::getMinute() {
  return ((this->getEpochTime() % 3600) / 60);
}

int NTPClient::getSecond() {
  return (this->getEpochTime() % 60);
}

String NTPClient::getFormattedYear() {
  unsigned long year = getYear();
  String yearStr = year < 10 ? "0" + String(year) : String(year);
  return yearStr;
}

String NTPClient::getFormattedTime() {
  unsigned long hour = getHour();
  String hourStr = hour < 10 ? "0" + String(hour) : String(hour);

  unsigned long minute = getMinute();
  String minuteStr = minute < 10 ? "0" + String(minute) : String(minute);

  unsigned long second = getSecond();
  String secondStr = second < 10 ? "0" + String(second) : String(second);

  return hourStr + ":" + minuteStr + ":" + secondStr;
}

String NTPClient::getFormattedHour() {
  unsigned long hour = getHour();
  String hourStr = hour < 10 ? "0" + String(hour) : String(hour);
  return hourStr;
}

String NTPClient::getFormattedMinute() {
  unsigned long minute = getMinute();
  String minuteStr = minute < 10 ? "0" + String(minute) : String(minute);
  return minuteStr;
}

String NTPClient::getFormattedSecond() {
  unsigned long second = getSecond();
  String secondStr = second < 10 ? "0" + String(second) : String(second);
  return secondStr;
}

String NTPClient::getFormattedDate() {
  unsigned long rawTime = this->getEpochTime() / 86400L;
  unsigned long days = 0, year = 1970;
  uint8_t month;
  static const uint8_t monthDays[]={31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

  while((days += (LEAP_YEAR(year) ? 366 : 365)) <= rawTime)
    year++;

  rawTime -= days - (LEAP_YEAR(year) ? 366 : 365); // now it is days in this year, starting at 0
  days=0;
  for (month = 0; month < 12; month++) {
    uint8_t monthLength;
    if (month == 1) { // Check february month condition
      monthLength = LEAP_YEAR(year) ? 29 : 28;
    } else {
      monthLength = monthDays[month];
    }
    if (rawTime < monthLength) break;
    rawTime -= monthLength;
  }
  String monthStr = ++month < 10 ? "0" + String(month) : String(month); // jan is month 1  
  String dayStr = ++rawTime < 10 ? "0" + String(rawTime) : String(rawTime); // day of month  
  return String(year) + "-" + monthStr + "-" + dayStr;
}

void NTPClient::end() {
  this->_udp->stop();

  this->_udpSetup = false;
}

void NTPClient::setTimeOffset(int timeOffset) {
  this->_timeOffset     = timeOffset;
}

void NTPClient::setUpdateInterval(unsigned long updateInterval) {
  this->_updateInterval = updateInterval;
}

void NTPClient::sendNTPPacket() {
  // set all bytes in the buffer to 0
  memset(this->_packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  this->_packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  this->_packetBuffer[1] = 0;     // Stratum, or type of clock
  this->_packetBuffer[2] = 6;     // Polling Interval
  this->_packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  this->_packetBuffer[12]  = 0x49;
  this->_packetBuffer[13]  = 0x4E;
  this->_packetBuffer[14]  = 0x49;
  this->_packetBuffer[15]  = 0x52;

  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  this->_udp->beginPacket(this->_poolServerName, 123); //NTP requests are to port 123
  this->_udp->write(this->_packetBuffer, NTP_PACKET_SIZE);
  this->_udp->endPacket();
}

void NTPClient::setEpochTime(unsigned long secs) {
  this->_currentEpoc = secs;
}
