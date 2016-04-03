
#ifndef HOUGHGRID_H
#define HOUGHGRID_H

#include <iostream>
#include <cmath>
#include "./../../occupancygrid/grid/grid.h"
#include "./../../ppmwriter/ppm_writer.h"

class HoughGrid
{
  public:
    static const int RADIUS_SIZE = (int)(Grid::GRID_SIZE * 1.414213562 / 8.0) + 1;
    static const int THETA_SIZE = 360; // 180/0.5
    static const int CENTER = Grid::GRID_SIZE/2;
    static const int ADDITION = (int)(Grid::GRID_SIZE * 0.7071067812) + 1;
    
    int Num_Points = 0;
    double D_THETA;
    
    unsigned char* map;
    
    //constructor/destructor
    HoughGrid();
    ~HoughGrid();
    
    void addHoughPoint(int x, int y);
    int getThetaSums(int* sums);
    
    void sendHoughToImage(char* filename);
    void setImagePixel(PPMwriter* w, int val);
};

#endif // HOUGHGRID_H
