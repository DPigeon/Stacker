#ifndef LAN_H
#define LAN_H

#include <Arduino.h>
#include <AsyncTCP.h>
#include <WiFi.h>

class Lan {
  
  private:
    const int serialBaud = 115200;
    const int port = 80;
    const char* ssId;
    const char* password;
    const char* hostname;
  
  public:
    Lan(char* hostname, char* ssId, char* password);
    void initConnection();
};

#endif
