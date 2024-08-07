/**
 * Arcade Game 
 * Stack dots on top of each other using a push button.
 */

#include <ESPAsyncWebServer.h>
#include <ESPmDNS.h>

#include "HighScores.h"
#include "LedMatrix.h"
#include "Lan.h"

const int serialBaud = 115200;
const int port = 80;
const int secondMs = 1000;
const char* hostname = "arcade";
String name = "";
String ipInUsed = "";
String buttonState = "0";
LedMatrix ledMatrix;

// Chrono
bool inGame = false;
unsigned long lastRefreshTime = 0;
unsigned long currentTime = 0;

AsyncWebServer server(port);
AsyncWebSocket ws("/ws");
HighScores hs;
Lan LAN(hostname);

const char index_html[] PROGMEM = R"rawliteral(
  <!DOCTYPE HTML>
  <html>
    <head>
      <title>
        Arcade
      </title>
      <meta name="viewport" content="width=device-width, initial-scale=1">
      <link rel="icon" href="data:,">
      <style>
      </style>
    </head>
    <body>
      <center>
        <h1>
          Arcade
        </h1>
        <a href="/highscores">HighScores</a>
        <br>
        <br>
        <label for="textbox-name">Name:</label>
        <input type="text" id="textbox-name" name="textbox-name">
        <br>
        <br>
        <b>Status</b>: <span id="clientState">%CLIENTSTATE%</span>
        <br>
        <b>Game</b>: <span id="gameState">%GAMESTATE%</span>
        <br>
        <b>Timer</b>: <span id="chrono">%CHRONO%</span>
        <br>
        <br>
        <button 
          id="stack-button" 
          type="button" 
          style="
            cursor:pointer;
            border:15px solid black;
            background-color:red;
            border-radius:50%;
            width:200px;
            height:200px;
          ">
        </button>
      </center>
      <script>
        var timer = 0;
        window.addEventListener('load', function() {
          var ws = new WebSocket(`ws://${window.location.hostname}/ws`);
          ws.onopen = function(event) {
            console.log('WS connection opened!');
            document.getElementById('clientState').innerHTML = "Connected";
          }
          ws.onclose = function(event) {
            console.log('WS connection closed!');
            document.getElementById('clientState').innerHTML = "Disconnected";
            if (location) {
              location.reload();
            }
          }
          ws.onerror = function(error) {
            console.log(error);
          };
          ws.onmessage = function(event) {
            if (event.data == "In Used") {
              document.getElementById('gameState').innerHTML = "In Used";
            } else if (event.data == "Ready") {
              document.getElementById('gameState').innerHTML = "Ready";
            } else if (event.data.startsWith("UpdateChrono:")) {
              timer = event.data.replace("UpdateChrono: ", "");
              document.getElementById('chrono').innerHTML = timer + " s";
            } else if (event.data == "StopChrono") {
              timer = 0;
              document.getElementById('chrono').innerHTML = timer + " s";
            }
          };
          document.getElementById('stack-button').addEventListener('click', function() {
            if (ws.readyState != 1 && location) { // Reset board but web still up
              location.reload();
            } else {
              var name = document.getElementById('textbox-name').value;
              if (name === "") {
                alert("Enter your name to play!");
              } else {
                ws.send('stack: ' + name);
              }
            }
          });
        });
	    </script>
    </body>
  </html>
)rawliteral";

const char highscores_html[] PROGMEM = R"rawliteral(
  <!DOCTYPE HTML>
  <html>
    <head>
      <title>
        HighScores
      </title>
      <script src="https://code.jquery.com/jquery-3.7.1.min.js"></script>
      <meta name="viewport" content="width=device-width, initial-scale=1">
      <link rel="icon" href="data:,">
      <style>
      </style>
    </head>
    <style>
      #highscores {
        font-family: Arial, Helvetica, sans-serif;
        border-collapse: collapse;
        width: 100%;
      }
      #highscores td, #highscores th {
        border: 1px solid #ddd;
        padding: 12px;
      }
      tr:nth-child(even) {
        background-color: #f2f2f2;
      }
      tr:hover {
        background-color: #ddd;
      }
      th {
        padding-top: 12px;
        padding-bottom: 12px;
        text-align: left;
        background-color: red;
        color: white;
      }
    </style>
    <body>
        <center>
          <h1>HighScores</h1>
          <a href="/">Play</a>
        </center>
        <br>
        <br>
        <table id="highscores">
          <tr>
            <th>Rank</th>
            <th>Name</th>
            <th>Time</th>
            <th>Score</th>
          </tr>
        </table>
      <script type="module">
        import { initializeApp } from "https://www.gstatic.com/firebasejs/10.7.1/firebase-app.js";
        import { getDatabase, ref, query, onValue, orderByChild, limitToFirst } from "https://www.gstatic.com/firebasejs/10.7.1/firebase-database.js";

        window.addEventListener('load', function() {
          const firebaseConfig = {
            databaseURL: "https://esp32-stacker-default-rtdb.firebaseio.com/"
          };

          const app = initializeApp(firebaseConfig);
          const db = getDatabase(app);
          const topLimit = 20;

          onValue(query(ref(db, 'users/'), orderByChild('score'), limitToFirst(topLimit)), function(snapshot) {
            if (snapshot.exists()) {
              var content = '';
              var index = 0;
              snapshot.forEach(function(data) {
                var i = index + 1;
                var score = -1 * data.val().score;
                index++;
                content += '<tr>';
                content += '<td>' + i + '</td>';
                content += '<td>' + data.key + '</td>';
                content += '<td>' + data.val().time + '</td>';
                content += '<td>' + score + '</td>';
                content += '</tr>';
              });
              $('#highscores').append(content);
            }
          }, { 
            onlyOnce: true 
          });
        });
      </script>
    </body>
  </html>
)rawliteral";

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
  if (var == "CHRONO") {
    char c[10];
    sprintf(c, "%ld s", lastRefreshTime / secondMs);
    return c;
  }
  return String();
}

void initWebServer() {
  LAN.initConnection();
  if (!MDNS.begin(hostname)) {
    Serial.println("Error setting up mDNS responder!");
    while (1) {
      delay(secondMs);
    }
  }
  Serial.println("mDNS Responder Started!");

  ws.onEvent(eventHandler);
  server.addHandler(&ws);

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", index_html, processor);
  });

  server.on("/highscores", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", highscores_html);
  });

  server.begin();
  MDNS.addService("http", "tcp", port);
}

void handleButton() {
  if (buttonState == "1") {
    ledMatrix.stackLed();
    buttonState = "0";
  }
  if (ledMatrix.getGameOver()) {
    ipInUsed = "";
    currentTime, lastRefreshTime = 0;
    ws.textAll("Ready");
    ledMatrix.setGameOver(false);
  }
}

void handleChrono() {
  if (!inGame && ipInUsed != "") {
    inGame = true;
  } else if (inGame && ipInUsed == "") {
    inGame = false;
    ws.textAll("StopChrono");
    if (hs.isReady()) {
      hs.add(name, 4, 4);
    }
  }
  if (inGame) {
    unsigned long t = currentTime - lastRefreshTime;
    if (t >= secondMs && t % secondMs == 0) { // Update every second on screen, check if mod secondMs to fix random millis() number
      lastRefreshTime += secondMs;
      char c[10];
      sprintf(c, "%ld", lastRefreshTime / secondMs);
      String msg = "UpdateChrono: ";
      msg += c;
      ws.textAll(msg);
    }
  }
}
                
void setup() {
  Serial.begin(serialBaud);
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
