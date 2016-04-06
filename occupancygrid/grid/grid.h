
#ifndef OG_GRID_
#define OG_GRID_

#include <iostream> //TODO:remove

class Grid {
  public:
    static const int GRID_SIZE = 801;   //size of one side of the grid (odd) 61 for good data structure
    static const char MAX_VALUE = 127;  //maximum value we can have in a grid square
    
    static const char UNKNOWN = 0;
    static const char OPEN = 1;
    static const char CLOSED = -10;
    static const char THRESHOLD = 0;
    static const char OPEN_MAX = MAX_VALUE - OPEN;
    static const char CLOSED_MIN = -MAX_VALUE - CLOSED;
    
    char *map;                          //occupancy grid for this node
    
    //constructor/destructor
    Grid();
    ~Grid();
    
    //set and get values in grid
    void setValue(int x, int y, char value);
    void changeValue(int x, int y, char value);
    bool thresholdOpenValue(int x, int y);
    char getValue(int x, int y);
    
    void openValue(int x, int y);
    void closeValue(int x, int y);
};

#endif