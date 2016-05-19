
#ifndef SONARARCHIVE_H
#define SONARARCHIVE_H

#include <cstdlib>
#include <cmath>

#include "./../occupancygrid/occupancygrid.h"

class SonarArchive
{
public:
  static const int SCALE = 10;  //number of cm per grid square
  static const int SONAR_WALL = 225;    //maximum range on sonar (not based on scale)
  static const int FRONT_CLIP = 100;    //maximum range of front sonars
  static const int SIDE_CLIP = 150;
  
  SonarArchive();
  ~SonarArchive();
  void addSonarScan(int* sonarDists, float x, float y, float head);
  OccupancyGrid* generateMap(float sliceAngle);
  
  void addPath(OccupancyGrid* grid);
  void rotateMap(float angle);
  
private:
  typedef struct SonarScan
  {
    unsigned short w, nw, ne, e;    //sonar distances
    bool wWall, eWall;
    float x, y;                     //position readings were made at
    float rotX, rotY;               //position after rotation
    float heading;                  //direction facing during measurements
    float rotHeading;               //heading set after rotation
    struct SonarScan* next;         //previous scan in chain
  } SonarScan;
  
  
  
private:
  SonarScan* scanList = nullptr;
  SonarScan* lastScan = nullptr;
  void getSonarCoords(SonarScan* scan, int* buffer);
};

#endif // SONARARCHIVE_H
