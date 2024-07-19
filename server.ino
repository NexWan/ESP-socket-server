#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "Coords.h"

// Just put whatever coordinates you want to start with
#define LAT 25.4417045590472
#define LON -100.99283456311456

const char *SSID = "SSID";
const char *PASSWORD = "PWD";

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

Coords coords;

void notifyClients(String message)
{
  ws.textAll(message);
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len)
{
  AwsFrameInfo *info = (AwsFrameInfo *)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT)
  {
    data[len] = 0;
    if (info->index == 0)
    {
      ws.textAll((char *)data);
      Serial.printf("Received text: %s\n", data);
    }
  }
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len)
{
  switch (type)
  {
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

void initWebSocket()
{
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}

String processor(const String &var)
{
  Serial.println(var);
  return "";
}

void simulateGpsMovement(char direction, float distance, float speed)
{
  // Earth's radius in meters
  const float earthRadius = 6378137.0;

  // Convert distance from meters to degrees
  float distanceInDegreesLat = (distance / earthRadius) * (180.0 / PI);
  float distanceInDegreesLon = distanceInDegreesLat / cos(coords.getLat() * PI / 180.0);

  if (direction == 'N')
  {
    coords.setLat(coords.getLat() + distanceInDegreesLat);
  }
  else if (direction == 'S')
  {
    coords.setLat(coords.getLat() - distanceInDegreesLat);
  }
  else if (direction == 'E')
  {
    coords.setLon(coords.getLon() + distanceInDegreesLon);
  }
  else if (direction == 'W')
  {
    coords.setLon(coords.getLon() - distanceInDegreesLon);
  }

  notifyClients("Lat: " + String(coords.getLat(), 6) + " Lon: " + String(coords.getLon(), 6));
}

void goToCoords(float lat, float lon)
{
  coords.setCoords(lat, lon);
  notifyClients("Lat: " + String(coords.getLat(), 6) + " Lon: " + String(coords.getLon(), 6));
}

void setup()
{

  // put your setup code here, to run once:
  Serial.begin(115200);
  coords.setCoords(LAT, LON);

  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  // Print local IP address and start web server
  Serial.println(WiFi.localIP());

  initWebSocket();

  server.begin();
}

void loop()
{
  // put your main code here, to run repeatedly:
  ws.cleanupClients();
  delay(1000);
  coords.printCoords();

  if (Serial.available() > 0)
  {
    String message = Serial.readStringUntil('\n');
    message.trim();
    if (message.length() > 0)
    {
      if (message.startsWith("GO"))
      {
        int firstSpace = message.indexOf(' ');
        int secondSpace = message.indexOf(' ', firstSpace + 1);
        if (firstSpace != -1 && secondSpace != -1)
        {
          String latStr = message.substring(firstSpace + 1, secondSpace);
          String lonStr = message.substring(secondSpace + 1);
          float lat = latStr.toFloat();
          float lon = lonStr.toFloat();
          goToCoords(lat, lon); // Modified to pass lat and lon as arguments
        }
      }
      else
      {
        simulateGpsMovement(message[0], message.substring(1).toFloat(), 0);
      }
    }
  }
}
