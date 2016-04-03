
#ifndef NAVIGATIONMAP_H_
#define NAVIGATIONMAP_H_

#include <iostream> //TODO: remove
#include <string>   //TODO: remove
#include <cstdlib>
#include <cmath>
#include <algorithm>

#include "grid/opengrid.h"
#include "grid/frontiergrid.h"
#include "./../../pngwriter/png_writer.h"

class NavigationMap
{
  public:
    //occupancy variables
    static const short OPEN = 1;
    static const short CLOSED = -10;
    static const char FRONTIER = 5;
    static const char UNFRONTIER = -5;
    static const char THRESHOLD = 0;
    
    //TODO: Make private
    //other variables
    OpenGrid *grid; //occupancy grid
    FrontierGrid *frontiers;    //frontier grid
    
    //constructor/destructor
    NavigationMap();
    ~NavigationMap();
    
    //functions for adding sonar slices to map
    void openSliceSide(int relX1, int relY1, int relX2, int relY2, float angle);
    void closeSliceSide(int relX1, int relY1, int relX2, int relY2, float angle);
    void openSliceFront(int relX1, int relY1, int relX2, int relY2, float angle);
    void closeSliceFront(int relX1, int relY1, int relX2, int relY2, float angle);
    
    void cleanFrontier();
    
    //output map as image
    void sendToImage(char* filename);
    void sendMapToImage(char* filename);
    void sendFrontierToImage(char* filename);
    
  private:
    //set a pixel in the image
    void setMapImagePixel(PngWriter* w, int x, int y, unsigned short value);
    void setFrontierImagePixel(PngWriter* w, int x, int y, unsigned char value);
    
    //functions for adding to map
    void openLine(int relX1, int relY1, int relX2, int relY2);
    void openFrontierLine(int relX1, int relY1, int relX2, int relY2);
    void closeFrontierLine(int relX1, int relY1, int relX2, int relY2);
    void openLineSide(int relX1, int relY1, int relX2, int relY2);
    void closeLineSide(int relX1, int relY1, int relX2, int relY2);
    void openLineFront(int relX1, int relY1, int relX2, int relY2);
    void closeLineFront(int relX1, int relY1, int relX2, int relY2);
};

#endif // NAVIGATIONMAP_H
