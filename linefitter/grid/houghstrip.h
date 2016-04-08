#ifndef HOUGHSTRIP_H
#define HOUGHSTRIP_H

#include <iostream>
#include <cmath>
#include "./../../occupancygrid/grid/grid.h"
#include "./../../ppmwriter/ppm_writer.h"

class HoughStrip
{
  public:
    static const int THETA_SIZE = 360; // 180/0.5
    static const int CENTER = Grid::GRID_SIZE/2;
    
    float D_THETA;
    
    float* SIN_ARRAY;
    float* COS_ARRAY;
    float* TAN_ARRAY;
    
    int* X1_MIN_ARRAY;
    int* Y1_MIN_ARRAY;
    int* X2_MIN_ARRAY;
    int* Y2_MIN_ARRAY;
    int* MIN1_VALUE_ARRAY;
    int* MIN2_VALUE_ARRAY;
    
    int* X1_MAX_ARRAY;
    int* Y1_MAX_ARRAY;
    int* X2_MAX_ARRAY;
    int* Y2_MAX_ARRAY;
    int* MAX1_VALUE_ARRAY;
    int* MAX2_VALUE_ARRAY;
    
    int* COUNT_MIN_ARRAY;
    int* COUNT_MAX_ARRAY;
    
    int Num_Points = 0;
    
    //constructor/destructor
    HoughStrip();
    ~HoughStrip();
    
    void addHoughPoint(int x, int y);
    int getThetaPeaks(int* sums);
    int getTanIndex(float tan);
};

#endif // HOUGHSTRIP_H
