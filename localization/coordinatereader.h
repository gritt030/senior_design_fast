
#ifndef COORDINATEREADER_H
#define COORDINATEREADER_H

#include <iostream>
#include <cstdlib>
#include <fstream>
#include <cmath>
#include <algorithm>

class CoordinateReader
{
  public:
    CoordinateReader(char* filename);
    CoordinateReader();
    void updateCoordsFile();
    void updateCoordsFileNew();
    void updateCoordsVirtual(int* coords);
    
    //get position data
    void getCurrentCoords(int* buffer);
    void getCurrentCoordError(int* buffer);
    
    //get rotation data
    void getCurrentRotation(float* buffer);
    float getYawError();
    
    //get sonar data
    void getCurrentSonarCoords(int* buffer);
    void getCurrentSonarDists(int* buffer);
    
    //new file format
    bool getCurrentPoseNew(double* buffer);
    bool getCurrentSonarsNew(int* buffer);
    
  private:
    std::ifstream coordFile;
    int* currentCoords;
    
    double* currentPos;
    int* currentSonar;
};

#endif // COORDINATEREADER_H
