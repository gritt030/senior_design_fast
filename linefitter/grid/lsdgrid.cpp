
#include "lsdgrid.h"

//constructor, create grid
LsdGrid::LsdGrid(int size) {
  this->GRID_SIZE = size;
  this->map = new unsigned char[GRID_SIZE * GRID_SIZE]();
}


//destructor, destroy grid
LsdGrid::~LsdGrid() {
  delete[] this->map;
}


//set a value in the grid to a character
void LsdGrid::setValue(int x, int y, unsigned char value){
  //bounds checking
  if ((x<0) || (y<0) || (x>=GRID_SIZE) || (y>=GRID_SIZE)) {
    return;
  }
  
  //set value
  this->map[y*GRID_SIZE + x] = value;
}


//get the value of a square in the grid
unsigned char LsdGrid::getValue(int x, int y){
  //bounds checking
  if ((x<0) || (y<0) || (x>=GRID_SIZE) || (y>=GRID_SIZE)) {
    return UNDEFINED;
  }
  
  //return value
  return this->map[y*GRID_SIZE + x];
}

