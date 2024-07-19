/*
    I couldn't make my GPS module work so I'm going to simulate the movement of the GPS module using the Coords class.
*/

#ifndef COORDS_H
#define COORDS_H

class Coords {
  public:
    Coords();
    Coords(float lat, float lon);
    float getLat();
    float getLon();
    void setLat(float lat);
    void setLon(float lon);
    void setCoords(float lat, float lon);
    void setCoords(Coords coords);
    Coords getCoords();
    void printCoords();
  private:
    float lat;
    float lon;
};

#endif