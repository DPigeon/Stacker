#include "AudioWebSocket.h"

AudioWebSocket::AudioWebSocket() {}

AudioWebSocket::AudioWebSocket(String host, String path, int port, bool enabled) {
    this->host = host;
    this->path = path;
    this->port = port;
    this->enabled = enabled;
}

void AudioWebSocket::connect() {
    if (enabled) {
        wsClient.begin(host, port, path);
        wsClient.setReconnectInterval(RECONNECT_INTERVAL_MS);
    }
}

void AudioWebSocket::send(String msg) {
    if (enabled) {
        wsClient.sendTXT(msg);
    }
}

void AudioWebSocket::loop() {
    if (enabled) {
        wsClient.loop();
    }
}