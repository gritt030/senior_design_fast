
#include "frontiergrid.h"

//constructor, create grid
FrontierGrid::FrontierGrid() {
  this->map = new unsigned char[OpenGrid::GRID_SIZE * OpenGrid::GRID_SIZE]();
}

//destructor, destroy grid
FrontierGrid::~FrontierGrid() {
  delete[] this->map;
}


//set a value in the grid to a character
void FrontierGrid::setValue(int x, int y, unsigned char value){
  //bounds checking
  if ((abs(x) > OpenGrid::GRID_BOUNDARY) || (abs(y) > OpenGrid::GRID_BOUNDARY)) {
    return;
  }
  
  //get index
  int index = x + OpenGrid::GRID_BOUNDARY;
  index *= OpenGrid::GRID_SIZE;
  index += OpenGrid::GRID_BOUNDARY - y;
  
  //set value
  this->map[index] = value;
}


//change a value in the grid by the amount passed in
void FrontierGrid::changeValue(int x, int y, char value){
  //bounds checking
  if ((abs(x) > OpenGrid::GRID_BOUNDARY) || (abs(y) > OpenGrid::GRID_BOUNDARY)) {
    return;
  }
  
  //get index
  int index = x + OpenGrid::GRID_BOUNDARY;
  index *= OpenGrid::GRID_SIZE;
  index += OpenGrid::GRID_BOUNDARY - y;
  
  //get current value
  unsigned short current = this->map[index];
  unsigned short orig = current;
  
  current += value;
  
  //check for overflow
  if (value > 0) {
    if (orig > current) current = FrontierGrid::MAX_VALUE;
  } else {
    if (orig < current) current = 0;
  }
  
  this->map[index] = current;
}


//get the value of a square in the grid
unsigned char FrontierGrid::getValue(int x, int y){
  //bounds checking
  if ((abs(x) > OpenGrid::GRID_BOUNDARY) || (abs(y) > OpenGrid::GRID_BOUNDARY)) {
    return (unsigned short)0;
  }
  
  //get index
  int index = x + OpenGrid::GRID_BOUNDARY;
  index *= OpenGrid::GRID_SIZE;
  index += OpenGrid::GRID_BOUNDARY - y;
  
  //return value
  return this->map[index];
}

