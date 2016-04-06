
#ifndef OG_OCCUPANCY_GRID_
#define OG_OCCUPANCY_GRID_

#include <iostream> //TODO: remove
#include <string>   //TODO: remove
#include <cstdlib>
#include <cmath>
#include <algorithm>

#include "grid/grid.h"
#include "./../ppmwriter/ppm_writer.h"

class OccupancyGrid {
  public:
    //occupancy variables
    static const char UNKNOWN = Grid::UNKNOWN;
    static const char OPEN = Grid::OPEN;
    static const char CLOSED = Grid::CLOSED;
    static const char THRESHOLD = Grid::THRESHOLD;
    
    //used to correct coordinates when setting values in grid
    static const int BOUNDARY = Grid::GRID_SIZE / 2;
    
    //Gaussian kernels to use to blur image
    static const int NUM_KERNELS = 9;
    static const int KERNELS[][NUM_KERNELS + 1];
    static const int KERNEL_SUMS[];
    
    //TODO: Make private
    //other variables
    Grid *grid; //occupancy grid
    
    //constructor/destructor
    OccupancyGrid();
    ~OccupancyGrid();
    
    //functions for adding to map
    bool openLine(int relX1, int relY1, int relX2, int relY2); //TODO: make this private
    void closeLine(int relX1, int relY1, int relX2, int relY2);
    
    //functions for adding sonar slices to map
    void openSlice(int relX1, int relY1, int relX2, int relY2, float angle);
    void closeSlice(int relX1, int relY1, int relX2, int relY2, float angle);
    
    //blur the map according to uncertainty
    void blurMapX(int uncertainty);
    void blurMapY(int uncertainty);
    
    //combine two occupancy maps
    void mergeMaps(OccupancyGrid* newGrid);
    void overlayMaps(OccupancyGrid* newGrid);

    //copy just the walls of the occupancy map into newGrid
    void getWallMap(OccupancyGrid* newGrid);
    //copy just the open areas of the occupancy map into newGrid
    void getOpenMap(OccupancyGrid* newGrid);
    
    //output map as image
    void sendToImage(char* filename, int x, int y);
    
    //theta used for cardinal directions
    double X_Cardinal=-1.0, Y_Cardinal=-1.0;
    
  private:
    //set a pixel in the image
    void setImagePixel(PPMwriter* w, char value);    
};

#endif