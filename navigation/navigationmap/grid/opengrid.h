
#ifndef OPENGRID_H_
#define OPENGRID_H_

#include <iostream> //TODO:remove

class OpenGrid
{
public:
    static const int GRID_SIZE = 801;   //size of one side of the grid (odd)
    static const int GRID_BOUNDARY = GRID_SIZE/2;
    static const unsigned short MAX_VALUE = 65535;  //maximum value we can have in a grid square
    unsigned short *map;                          //occupancy grid for this node
    
    //constructor/destructor
    OpenGrid();
    ~OpenGrid();
    
    //set and get values in grid
    void setValue(int x, int y, unsigned short value);
    void changeValue(int x, int y, short value);
    unsigned short getValue(int x, int y);
};

#endif // OPENGRID_H
