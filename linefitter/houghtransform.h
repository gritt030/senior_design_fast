#ifndef HOUGHTRANSFORM_H
#define HOUGHTRANSFORM_H

#include <cstdlib>
#include <chrono>
#include <vector>

#include "./../occupancygrid/occupancygrid.h"
#include "./grid/houghgrid.h"
#include "./grid/houghpeak.h"

class HoughTransform
{
public:
  HoughTransform(OccupancyGrid* grid);
  HoughTransform(int numPoints, std::vector<int>* xpos, std::vector<int>* ypos);
  ~HoughTransform();
  
  float getYCardinal();
  float getXCardinal();
      
private:
  float X_Cardinal, Y_Cardinal;
  bool isDone = false;
  
  
  int numpts = 0;
  
  HoughGrid* houghGrid = nullptr;
  HoughPeak* houghPeak = nullptr;
  OccupancyGrid* grid = nullptr;
  
  int numPoints = 0;
  std::vector<int> xPos;
  std::vector<int> yPos;
  
  void performHoughTransform();
  void performHoughPeaks();
  void detectCardinalDirections();
};

#endif // HOUGHTRANSFORM_H
