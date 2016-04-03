
#ifndef OG_GRID_
#define OG_GRID_

#include <iostream> //TODO:remove

class Grid {
  public:
    static const int GRID_SIZE = 801;   //size of one side of the grid (odd) 61 for good data structure
    static const char MAX_VALUE = 127;  //maximum value we can have in a grid square
    char *map;                          //occupancy grid for this node
    
    //constructor/destructor
    Grid();
    ~Grid();
    
    //set and get values in grid
    void setValue(int x, int y, char value);
    void changeValue(int x, int y, char value);
    char getValue(int x, int y);
};

#endif