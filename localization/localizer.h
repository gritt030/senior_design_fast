
#ifndef LOCALIZER_H
#define LOCALIZER_H

#include "coordinatereader.h"

class Localizer
{
  public:
    static const int SCALE = 100;        //cm per grid square
    static const int SONAR_MAX = 400;    //maximum range on sonar (not based on scale)
    
    //constructor
    Localizer(CoordinateReader* r);
    
    //trigger updates to coordinates
    void triggerUpdate();
    
    //get position/orientation data
    void getPosition(int* buffer);
    void getPositionError(int* buffer);
    void getRotation(float* buffer);
    float getYawError();
    
    //get sonar data
    void getWSonarPosition(int* buffer);
    void getNWSonarPosition(int* buffer);
    void getNESonarPosition(int* buffer);
    void getESonarPosition(int* buffer);
    void getSonarInRange(bool* buffer);
    
    //get raw data
    void getRawPosition(double* buffer);
    void getRawHeading(double* buffer);
    void getRawSonarDists(int* buffer);
  
  private:
    CoordinateReader* reader;
    int* currentCoords;         //x, y, z, x err, y err
    int* currentSonarCoords;    //wx, wy, wz, nwx, nwy, nwz, nex, ney, nez, ex, ey, ez
    int* currentSonarDists;     //wd, nwd, ned, ed
    float* currentRotation;     //roll, pitch, yaw, yaw err
    
    void updateCoords();
    void updateSonars();
    void updateRotation();
};

#endif // LOCALIZER_H
