#ifndef HIGHSCORES_H
#define HIGHSCORES_H

#include <Arduino.h>
#include <Firebase_ESP_Client.h>

class HighScores {

private:
  String DB_URL = "DB_URL"; // https://randomnerdtutorials.com/esp32-firebase-realtime-database/
  FirebaseData data;
  FirebaseAuth auth;
  FirebaseConfig config;

public:
  HighScores();
  bool isReady();
  void add(String name, int time, int score);
  int getScoreByName(String name);
};

#endif