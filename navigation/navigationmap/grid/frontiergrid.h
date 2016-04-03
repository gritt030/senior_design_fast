
#ifndef FRONTIERGRID_H
#define FRONTIERGRID_H

#include "opengrid.h"

class FrontierGrid
{
public:
  static const unsigned char MAX_VALUE = 255;  //maximum value we can have in a grid square
  unsigned char *map;                          //occupancy grid for this node
    
  //constructor/destructor
  FrontierGrid();
  ~FrontierGrid();
    
  //set and get values in grid
  void setValue(int x, int y, unsigned char value);
  void changeValue(int x, int y, char value);
  unsigned char getValue(int x, int y);
};

#endif // FRONTIERGRID_H
