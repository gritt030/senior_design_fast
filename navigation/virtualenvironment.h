#ifndef VIRTUALENVIRONMENT_H_
#define VIRTUALENVIRONMENT_H_

#include <iostream>
#include <cmath>

class VirtualEnvironment
{
  public:
    //constructor
    VirtualEnvironment();
    
    void setPosition(int x, int y);
    void setRotation(float angle);
    void changePosition(int dx, int dy);
    void changeRotation(float dangle);
    
    void getPosition(int* buffer);
    void getSonarCoords(int* buffer);
    void getSonarRanges(int* buffer);
    
    void loadHallway(int x1, int y1, int x2, int y2);
    
    void getCurrentCoordinates(int* buffer);
    
  private:
    char* map;
    int mapSize = 5000;
    int X, Y;
    float Angle, Degrees;
    
    int getNWSonar();
    int getNESonar();
    int getWeSonar();
    int getEaSonar();
};

#endif // VIRTUALENVIRONMENT_H
