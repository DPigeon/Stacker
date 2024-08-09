/**
 * HighScores class with Firebase
 * Accessible at /highscores
 */

#ifndef HIGHSCORES_H
#define HIGHSCORES_H

#include <Arduino.h>
#include <Firebase_ESP_Client.h>

#include "Config.cpp"

class HighScores {

private:
  bool enabled;
  FirebaseData data;
  FirebaseAuth auth;
  FirebaseConfig config;

public:
  HighScores(bool enabled);
  bool isReady();
  void add(String name, int time, int score);
  int getScoreByName(String name);
};

#endif