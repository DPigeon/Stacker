#ifndef LAN_H
#define LAN_H

#include <Arduino.h>
#include <AsyncTCP.h>
#include <SPIFFS.h>
#include <WiFi.h>

class Lan {
  
  private:
    const char* hostname;
  
  public:
    Lan(const char* hostname);
    void initConnection();
    String readSpiffs(String path);
};

#endif
