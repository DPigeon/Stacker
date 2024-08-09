#include <Arduino.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ESPmDNS.h>
#include <WiFi.h>
#include <XT_DAC_Audio.h>

#include "SoundData.h"

#define AUDIO_PIN 25
#define BAUD 115200

const char* hostname = "stacker-api";
const char* ssid = "SSID";
const char* password = "pass";

int port = 80;
AsyncWebServer server(port);
AsyncWebSocket ws("/ws");
int defaultVolume, volume = 25;

XT_DAC_Audio_Class DacAudio(AUDIO_PIN, 0); // Use GPIO 25, one of the 2 DAC pins and timer 0

XT_Wav_Class pressStartToPlayAudio(pressStartToPlay);
XT_Wav_Class moveAudio(moveSound);
XT_Wav_Class cornerMoveAudio(cornerMoveSound);
XT_Wav_Class stackAudio(stackSound);
XT_Wav_Class weeEuuAudio(weeEuuSound);
XT_Wav_Class wereHeadingUpAudio(wereHeadingUpSound);
XT_Wav_Class itsGettingHighNowAudio(itsGettingHighNowSound);
XT_Wav_Class ohNoAudio(ohNoSound);
XT_Wav_Class gameOverAudio(gameOverSound);   
XT_Wav_Class gameOverVoiceAudio(gameOverVoiceSound);

const int numAudio = 10;
XT_Wav_Class sounds[numAudio] = {
    pressStartToPlayAudio,  // 0
    moveAudio,              // 1
    cornerMoveAudio,        // 2
    stackAudio,             // 3
    weeEuuAudio,            // 4
    wereHeadingUpAudio,     // 5
    itsGettingHighNowAudio, // 6
    ohNoAudio,              // 7
    gameOverAudio,          // 8
    gameOverVoiceAudio      // 9
};

const char index_html[] PROGMEM = R"rawliteral(
    <!DOCTYPE HTML>
    <html>
        <head>
            <title>
            API
            </title>
            <meta name="viewport" content="width=device-width, initial-scale=1">
            <style>
            </style>
        </head>
        <body>
            <center>
                <p>Volume</p>
                <input type="range" min="0" max="100" value="25" id="slider">
                <p id="slider-value">%AUDIOVOLUME%</p>
            </center>
            <script>
                let slider = document.getElementById('slider');
                slider.addEventListener('input', function() {
                    document.getElementById('slider-value').innerHTML = slider.value;
                });
                slider.addEventListener('change', function() {
                    var ws = new WebSocket(`ws://${window.location.hostname}/ws`);
                    ws.onopen = function(event) {
                        console.log('WS connection opened!');
                        ws.send('Volume: ' + slider.value);
                        ws.close();
                    }
                });
            </script>
        </body>
    </html>
)rawliteral";

void playSound(int id) {
    if (!sounds[id].Playing && (id >= 0 && id < numAudio)) {
        DacAudio.Play(&sounds[id]);
    }
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t length) {
    AwsFrameInfo *info = (AwsFrameInfo*)arg;
    if (info->final && info->index == 0 && info->len == length && info->opcode == WS_TEXT) {
        data[length] = 0;
        char* d = (char*)data;
        if (strstr(d, "Volume: ")) {
            char value[4];
            strncpy(value, d+(7), 3);
            volume = atoi(value);
            DacAudio.DacVolume = volume;
        } else {
            playSound(atoi(d));
        }
    }
}

void eventHandler(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
    switch (type) {
        case WS_EVT_CONNECT:
            Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
            break;
        case WS_EVT_DISCONNECT:
            Serial.printf("WebSocket client #%u disconnected\n", client->id());
            break;
        case WS_EVT_DATA:
            handleWebSocketMessage(arg, data, len);
            break;
        case WS_EVT_PONG:
        case WS_EVT_ERROR:
            break;
    }
}

String processor(const String& var) {
    if (var == "AUDIOVOLUME") {
        char c[3];
        sprintf(c, "%ld", volume);
        return c;
    }
    return String();
}

void setup() {
    Serial.begin(BAUD);

    WiFi.begin(ssid, password);
  
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print("Connecting...");
    }
    
    Serial.println("");
    Serial.println("Connected!");
    

    if (!MDNS.begin(hostname)) {
        Serial.println("Error setting up mDNS responder!");
        while (1) {
            delay(1000);
        }
    }
    Serial.println("mDNS Responder Started!");

    ws.onEvent(eventHandler);
    server.addHandler(&ws);

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send_P(200, "text/html", index_html, processor);
    });

    server.begin(); // HTTPS? https://github.com/hoeken/PsychicHttp but memory limit with SSL...
    MDNS.addService("http", "tcp", port);

    DacAudio.DacVolume = defaultVolume;
}

void loop() {
    ws.cleanupClients();
    DacAudio.FillBuffer();
}
