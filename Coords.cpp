#include "Coords.h"
#include <Arduino.h>

Coords::Coords() {
  lat = 0;
  lon = 0;
}

Coords::Coords(float lat, float lon) {
  this->lat = lat;
  this->lon = lon;
}

float Coords::getLat() {
  return lat;
}

float Coords::getLon() {
  return lon;
}

void Coords::setLat(float lat) {
  this->lat = lat;
}

void Coords::setLon(float lon) {
  this->lon = lon;
}

void Coords::setCoords(float lat, float lon) {
  this->lat = lat;
  this->lon = lon;
}

void Coords::setCoords(Coords coords) {
  this->lat = coords.getLat();
  this->lon = coords.getLon();
}

Coords Coords::getCoords(){
    return Coords(lat, lon);
}

void Coords::printCoords() {
  Serial.print("Lat: ");
  Serial.print(lat, 6);
  Serial.print(" Lon: ");
  Serial.println(lon, 6);
}
