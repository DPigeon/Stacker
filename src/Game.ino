/**
 * Arcade Game 
 * Stack dots on top of each other using a push button.
 * Board: https://www.electronicshub.org/esp32-pinout/
 */

#include <ESPAsyncWebServer.h>
#include <ESPmDNS.h>
#include <SPIFFS.h>

#include "Config.cpp"
#include "HighScores.h"
#include "LedMatrix.h"
#include "Lan.h"

String name = "", ipInUsed = "";
String buttonState = "0";

bool inGame = false;
unsigned long lastRefreshTime, currentTime = 0;

AsyncWebServer server(PORT);
AsyncWebSocket ws(WS_PATH);
HighScores hs(HS_ENABLED);
LedMatrix ledMatrix;
Lan LAN(HOSTNAME);

void verifySpiffs() {
  if (!SPIFFS.begin(true)) {
    Serial.println("An error has occurred while mounting SPIFFS!");
    return;
  }
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t length, String ip) { // https://lastminuteengineers.com/esp32-websocket-tutorial/
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == length && info->opcode == WS_TEXT) {
    data[length] = 0;
    if (strstr((char*)data, "stack")) {
      String d = (char*)data;
      name = d.substring(6); // 'stack: ...'
      ipInUsed = ip;
      buttonState = 1;
      ws.textAll("In Used");
    }
  }
}

void eventHandler(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t length) {
  String ip = client->remoteIP().toString();
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), ip);
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      break;
    case WS_EVT_DATA:
      if (ipInUsed == "" || ipInUsed == ip) { // One plays at a time
        handleWebSocketMessage(arg, data, length, ip);
      }
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}

String processor(const String& var) {
  if (var == "CLIENTSTATE") {
    return "Unknown";
  }
  if (var == "GAMESTATE") {
    return ipInUsed != "" ? "In Used" : "Ready";
  }
  if (var == "SCORE") {
    char c[5];
    sprintf(c, "%ld", ledMatrix.getScore());
    return c;
  }
  if (var == "CHRONO") {
    char c[10];
    sprintf(c, "%ld s", lastRefreshTime / SECOND_MS);
    return c;
  }
  return String();
}

void initWebServer() {
  LAN.initConnection();
  if (!MDNS.begin(HOSTNAME)) {
    Serial.println("Error setting up mDNS responder!");
    while (1) {
      delay(SECOND_MS);
    }
  }
  Serial.println("mDNS Responder Started!");

  ws.onEvent(eventHandler);
  server.addHandler(&ws);

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });

  server.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/favicon.png", "image/png");
  });

  server.on("/highscores", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/highscores.html", String(), false);
  });

  server.on("/info", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/info.html", String(), false);
  });

  server.begin(); // HTTPS? https://github.com/hoeken/PsychicHttp but memory limit with SSL...
  MDNS.addService("http", "tcp", PORT);
}

void updateScoreInUI() {
  char c[5];
  sprintf(c, "%ld", ledMatrix.getScore());
  String msg = "UpdateScore: ";
  msg += c;
  ws.textAll(msg);
}

void handleButton() {
  if (buttonState == "1") {
    ledMatrix.stackLed();
    buttonState = "0";
    updateScoreInUI();
  }
  if (ledMatrix.getGameOver()) {
    ipInUsed = "";
    currentTime, lastRefreshTime = 0;
    ws.textAll("Ready");
    ledMatrix.setGameOver(false);
  }
}

void handleChrono() {
  if (currentTime > MAX_TIMER_MS) { // Avoid overflow
    currentTime, lastRefreshTime = 0;
  }
  if (!inGame && ipInUsed != "") {
    inGame = true;
  } else if (inGame && ipInUsed == "") {
    inGame = false;
    ws.textAll("StopChrono");
    if (hs.isReady()) {
      hs.add(name, lastRefreshTime / SECOND_MS, ledMatrix.getScore());
    }
  }
  if (inGame) {
    unsigned long t = currentTime - lastRefreshTime;
    if (t >= SECOND_MS && t % SECOND_MS == 0) { // Update every second on screen, check if mod SECOND_MS to fix random millis() number
      lastRefreshTime += SECOND_MS;
      char c[10];
      sprintf(c, "%ld", lastRefreshTime / SECOND_MS);
      String msg = "UpdateChrono: ";
      msg += c;
      ws.textAll(msg);
    }
  }
}
                
void setup() {
  Serial.begin(SERIAL_BAUD);
  verifySpiffs();
  initWebServer();
  ledMatrix.init();
}

void loop() {
  ws.cleanupClients();
  ledMatrix.updateTimer();
  currentTime = millis();
  handleButton();
  handleChrono();
  ledMatrix.drawGame();

  #ifdef DEBUG
    Serial.print(xLed);
    Serial.print("\n");
  #endif
}
