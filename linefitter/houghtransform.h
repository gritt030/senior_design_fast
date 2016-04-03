#ifndef HOUGHTRANSFORM_H
#define HOUGHTRANSFORM_H

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
  
  HoughGrid* houghGrid;
  OccupancyGrid* grid;
  
  void performHoughTransform();
  void detectCardinalDirections();
};





#endif // HOUGHTRANSFORM_H
