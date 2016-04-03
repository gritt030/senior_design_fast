
#ifndef SONARARCHIVE_H
#define SONARARCHIVE_H

#include <cmath>

#include "./../occupancygrid/occupancygrid.h"

class SonarArchive
{
public:
  //percent of distance traveled that error is
  double POSITION_ERROR = 0.01;
  //percent of angles turned that error is
  double HEADING_ERROR = 0.01;
  
  static const int SCALE = 10;  //number of cm per grid square
  static const int SONAR_MAX = 325;    //maximum range on sonar (not based on scale)
  
  SonarArchive();
  ~SonarArchive();
  void addSonarScan(int* sonarDists, double x, double y, double xErr, double yErr, double head, double headErr);
  void removeSonarScan();
  OccupancyGrid* generateMap();
  
  void shiftScans();
  void addPath(OccupancyGrid* grid);
  void rotateMap(double angle);
  
  void sortScans();
  void sortScansX();
  void sortScansY();
  void sortXsortYScans();
  void reverseScans();
  void printScans();
  
  
  
private:
  typedef struct SonarScan
  {
    unsigned short w, nw, ne, e;    //sonar distances
    double x, y;                    //position readings were made at
    double heading;                 //direction facing during measurements
    double xErr, yErr, headErr;     //errors in position and heading
    struct SonarScan* previous;     //previous scan in chain
  } SonarScan;
  
  
  
private:
  SonarScan* prevScan = nullptr;
  void propagateXPosError();
  void propagateYPosError();
  void propagateHeadError();
  void getSonarCoords(SonarScan* scan, int* buffer);
  double getSonarAngles(SonarScan* scan);
};

#endif // SONARARCHIVE_H
