
#ifndef HOUGHGRID_H
#define HOUGHGRID_H

#include <iostream>
#include <cmath>
#include "./../../occupancygrid/grid/grid.h"
#include "./../../ppmwriter/ppm_writer.h"

class HoughGrid
{
  public:
    static const char OFFSET = 0;
    static const int RADIUS_SIZE = ((int)(Grid::GRID_SIZE * 1.414213562) >> OFFSET) + 1;
    static const int THETA_SIZE = 360; // 180/0.5
    static const int CENTER = Grid::GRID_SIZE/2;
    static const int ADDITION = (RADIUS_SIZE >> 1) + 1;
    
    float D_THETA;
    
    float* SIN_ARRAY;
    float* COS_ARRAY;
    
    unsigned short* map;
    int Num_Points = 0;
    
    //constructor/destructor
    HoughGrid();
    ~HoughGrid();
    
    void addHoughPoint(int x, int y);
    int getThetaPeaks(int* sums);
    
    void sendHoughToImage(char* filename);
    void setImagePixel(PPMwriter* w, unsigned short val);
};

#endif // HOUGHGRID_H
