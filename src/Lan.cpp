#include "Lan.h"

Lan::Lan(const char* hostname) {
  this->hostname = hostname;
}

void Lan::initConnection() {
  WiFi.mode(WIFI_STA); // Set WiFi to station mode and disconnect from an AP if it was previously connected.
  WiFi.setHostname(hostname);
  String ssId = readSpiffs("/wifi-user.txt");
  String password = readSpiffs("/wifi-pass.txt");
  WiFi.begin(ssId, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected!");
}

String Lan::readSpiffs(String path) {
  File file = SPIFFS.open(path);
  if (!file) {
    Serial.println("LAN: Failed to open file for reading!");
    return "Incorrect";
  }
  String text = file.readString();
  file.close();
  return text;
}