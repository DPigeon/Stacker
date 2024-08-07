#ifndef LAN_H
#define LAN_H

#include <Arduino.h>
#include <AsyncTCP.h>
#include <WiFi.h>

class Lan {
  
  private:
    const int port = 80;
    const String ssId = "SSID";
    const String password = "";
    const char* hostname;
  
  public:
    Lan(const char* hostname);
    void initConnection();
};

#endif
