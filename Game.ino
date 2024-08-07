/**
 * Arcade Game 
 * Stack dots on top of each other using a push button.
 */

#include <ESPAsyncWebServer.h>
#include <ESPmDNS.h>

#include "Button.h"
#include "LedMatrix.h"
#include "Lan.h"

LedMatrix ledMatrix;

String buttonState = "0";

char* ssId = "SSID";
char* password = "";
const int port = 80;
AsyncWebServer server(port);
Lan LAN(ssId, ssId, password);
const char index_html[] PROGMEM = R"rawliteral(
  <!DOCTYPE HTML>
  <html>
    <head>
      <title>
        Doodle
      </title>
      <meta name="viewport" content="width=device-width, initial-scale=1">
      <link rel="icon" href="data:,">
      <style>
      </style>
    </head>
    <body>
      <h2>
        Doodle's Arcade
      </h2>
      <form>
        <br>
        <br>
        <button type="button" onclick="tapButton()" onmouseup="releaseButton()">Tap</button>
      </form>
      <script>
        function tapButton() {
          var xhr = new XMLHttpRequest();
          xhr.open("GET", "/stacker?tap=1", true);
          xhr.send();
        }
        function releaseButton() {
          var xhr = new XMLHttpRequest();
          xhr.open("GET", "/stacker?tap=0", true);
          xhr.send();
        }
      </script>
    </body>
  </html>
)rawliteral";

void initWebServer() {
  LAN.initConnection();
  if (!MDNS.begin(ssId)) {
    Serial.println("Error setting up mDNS responder!");
    while (1) {
      delay(1000);
    }
  }
  Serial.println("mDNS Responder Started!");

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", index_html);
  });

  server.on("/stacker", HTTP_GET, [](AsyncWebServerRequest *request) {
    const char* param = "tap";
    if (request->hasParam(param)) {
      buttonState = request->getParam(param)->value();
      Serial.println(buttonState);
    }
  });

  server.begin();
  MDNS.addService("http", "tcp", port);
}
                
void setup() { 
  Serial.begin(9600);
  ledMatrix.init();
}

void loop() {
  ledMatrix.updateTimer();
  handleButton();
  ledMatrix.drawGame();

  #ifdef DEBUG
    Serial.print(xLed);
    Serial.print("\n");
  #endif
}

void handleButton() {
  if (buttonState == "1") {
    if (ledMatrix.getGameOver()) ledMatrix.reset();
    ledMatrix.stackLed();
  }
}
