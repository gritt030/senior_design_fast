#ifndef HOUGHTRANSFORM_H
#define HOUGHTRANSFORM_H

#include <chrono>

#include "./../occupancygrid/occupancygrid.h"
#include "./grid/houghgrid.h"

class HoughTransform
{
public:
  HoughTransform(OccupancyGrid* grid);
  ~HoughTransform();
  
  double getYCardinal();
  double getXCardinal();
    
private:
  double X_Cardinal, Y_Cardinal;
  bool isDone = false;
  static const int FIT = 20; // 10/0.5
  
  HoughGrid* houghGrid;
  OccupancyGrid* grid;
  
  void performHoughTransform();
  void detectCardinalDirections();
};





#endif // HOUGHTRANSFORM_H
