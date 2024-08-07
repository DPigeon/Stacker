#include "Lan.h"

Lan::Lan(const char* hostname) {
  this->hostname = hostname;
}

void Lan::initConnection() {
  const int waitDelay = 1000;
  WiFi.mode(WIFI_STA); // Set WiFi to station mode and disconnect from an AP if it was previously connected.
  WiFi.setHostname(hostname);
  WiFi.begin(ssId, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(waitDelay);
    Serial.println("Connecting to WiFi..");
  }
}