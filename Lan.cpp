#include "Wifi.h"

Lan::Lan(char* hostname, char* ssId, char* password) {
  this->hostname = hostname;
  this->ssId = ssId;
  this->password = password;
}

void Lan::initConnection() {
  const int waitDelay = 1000;
  Serial.begin(serialBaud);
  WiFi.mode(WIFI_STA); // Set WiFi to station mode and disconnect from an AP if it was previously connected.
  WiFi.setHostname(hostname);
  WiFi.begin(ssId, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(waitDelay);
    Serial.println("Connecting to WiFi..");
  }
}