
#ifndef HOUGHPEAK_H
#define HOUGHPEAK_H

#include <iostream>
#include <cmath>
#include "./../../occupancygrid/grid/grid.h"

class HoughPeak
{
  public:
    static const int RADIUS_SIZE = (int)(Grid::GRID_SIZE * 1.414213562) + 1;
    static const int THETA_SIZE = 16;       //number of bins making up PEAK_DEGREES
    static const int PEAK_DEGREES = 8;     //number of degrees around peak to use
    static const int THETA_BOUND = THETA_SIZE * 2 + 1;
    static const int CENTER = Grid::GRID_SIZE/2;
    static const int ADDITION = (RADIUS_SIZE >> 1) + 1;
    
    float D_THETA;
    
    float* SIN_ARRAY;
    float* COS_ARRAY;
    
    unsigned short* peak1map;
    unsigned short* peak2map;
    
    int Num_Points = 0;
    
    //constructor/destructor
    HoughPeak(float peak1);
    ~HoughPeak();
    
    void addHoughPoint(int x, int y);
    int getThetaPeaks(int* sumpeak1, int* sumpeak2);
};

#endif // HOUGHPEAK_H
