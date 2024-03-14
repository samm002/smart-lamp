#ifndef WIFI_SETUP
#define WIFI_SETUP

#include <WiFi.h>

class Wifi {
  private:
    const char* _ssid;
    const char* _password;
  
  public:
    Wifi(const char* ssid, const char* password);
    
    uint8_t wifiStatus;
    unsigned long previousWaitTime;
    unsigned long connectingInterval;
    unsigned long currentMillis;
    
    void setup();
};

#endif