#include <TinyGPS++.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

#define GPS_BAUDRATE 9600


const char* SSID = "your_ssid";
const char* PASSWORD = "your_password";

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

TinyGPSPlus gps; // The TinyGPS++ object

void notifyClients(String message) {
  ws.textAll(message);
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;
    if (info->index == 0) {
      ws.textAll((char*)data);
      Serial.printf("Received text: %s\n", data);
    }
  }
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
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

void initWebSocket() {
  ws.onEvent(onEvent);
  server.addHandler(&ws);
} 

String processor(const String& var) {
  Serial.println(var);
  return "";
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial2.begin(GPS_BAUDRATE);

  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  // Print local IP address and start web server
  Serial.println(WiFi.localIP());

  initWebSocket();

  server.begin();

}

void loop() {
  // put your main code here, to run repeatedly:
  ws.cleanupClients();
  delay(1000);

  if(Serial.available() > 0){
    String message = Serial.readStringUntil('\n');

    message.trim();

    if(message.length() > 0){
      ws.textAll(message);
    }
  }

  if(Serial2.available() > 0){
    if(gps.encode(Serial2.read())){
      if(gps.location.isValid()){
        String message = String(gps.location.lat()) + "," + String(gps.location.lng());
        Serial.println(message);
      }
    }
  }

}
