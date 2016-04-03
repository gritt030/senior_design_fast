#ifndef LSDGRID_H
#define LSDGRID_H

class LsdGrid
{
public:
  int GRID_SIZE;
  static const unsigned char MAX_VALUE = 255;  //maximum value we can have in a grid square
  static const unsigned char UNDEFINED = 255;
  
  unsigned char* map;
  
  LsdGrid(int size);
  ~LsdGrid();
  
  //set and get values in grid
  void setValue(int x, int y, unsigned char value);
  unsigned char getValue(int x, int y);
};

#endif // LSDGRID_H
