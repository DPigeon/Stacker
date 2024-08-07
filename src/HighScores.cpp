#include "HighScores.h"

HighScores::HighScores() {
  config.database_url = DB_URL;
  config.signer.test_mode = true;
  Firebase.reconnectNetwork(true);
  data.setBSSLBufferSize(4096 /* Rx buffer size in bytes from 512 - 16384 */, 1024 /* Tx buffer size in bytes from 512 - 16384 */);
  data.setResponseSize(2048); // Limit the size of response payload to be collected in FirebaseData
  Firebase.begin(&config, &auth);
}

bool HighScores::isReady() {
  return Firebase.ready();
}

void HighScores::add(String name, int time, int score) {
  if (!name.isEmpty() && time > 0 && score > 0 && score > getScoreByName(name)) { // Adding new scores
    FirebaseJson json;
    json.set("time", time);
    json.set("score", -1 * score); // Will be negative for desc sorting
    String path = "users/";
    path += name;
    Serial.printf(
        "Set JSON... %s\n",
        Firebase.RTDB.set(
            &data,
            path,
            &json
        ) ? "OK" : data.errorReason().c_str()
    );
  }
}

int HighScores::getScoreByName(String name) {
  String path = "users/";
  path += name;
  path += "/score";
  int score = 0;
  Serial.printf(
        "Get score.., %s\n",
        Firebase.RTDB.getInt(
          &data,
          path,
          &score
        ) ? "OK" : data.errorReason().c_str()
  );
  return -1 * score;
}