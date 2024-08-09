/**
 * Audio WebSocket class that handles interactions with audio API
 */

#ifndef AUDIOWEBSOCKET_H
#define AUDIOWEBSOCKET_H

#include <Arduino.h>
#include <WebSocketsClient.h>

#define RECONNECT_INTERVAL_MS 5000 

class AudioWebSocket {
  
  private:
    String host;
    String path;
    int port;
    bool enabled;
    WebSocketsClient wsClient;
  
  public:
    AudioWebSocket();
    AudioWebSocket(String host, String path, int port, bool enabled);
    void connect();
    void send(String msg);
    void loop();
};

#endif
