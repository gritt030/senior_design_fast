
#include "grid.h"

//constructor, create grid
Grid::Grid() {
  this->map = new char[GRID_SIZE * GRID_SIZE]();
}

//destructor, destroy grid
Grid::~Grid() {
  delete[] this->map;
}


//set a value in the grid to a character
void Grid::setValue(int x, int y, char value){
  //bounds checking
  if ((x<0) || (y<0) || (x>=Grid::GRID_SIZE) || (y>=Grid::GRID_SIZE)) {
    std::cout << "BAD SET!\n";
    return;
  }
  
  //set value
  this->map[y*Grid::GRID_SIZE + x] = value;
}


//change a value in the grid by the amount passed in
void Grid::changeValue(int x, int y, char value){
  //bounds checking
  if ((x<0) || (y<0) || (x>=Grid::GRID_SIZE) || (y>=Grid::GRID_SIZE)) {
    std::cout << "BAD CHANGE!\n";
    return;
  }
  
  //int index = y*Grid::GRID_SIZE + x;
  char current = this->map[y*Grid::GRID_SIZE + x];
  
  if (value > 0) {
    if (current <= (Grid::MAX_VALUE - value)) {
      this->map[y*Grid::GRID_SIZE + x] = value + current;
    } else this->map[y*Grid::GRID_SIZE + x] = Grid::MAX_VALUE;
  } else {
    if (current >= (-Grid::MAX_VALUE-value)) {
      this->map[y*Grid::GRID_SIZE + x] = value + current;
    } else this->map[y*Grid::GRID_SIZE + x] = -Grid::MAX_VALUE;
  }
}


//get the value of a square in the grid
char Grid::getValue(int x, int y){
  //bounds checking
  if ((x<0) || (y<0) || (x>=Grid::GRID_SIZE) || (y>=Grid::GRID_SIZE)) {
    std::cout << "BAD GET!\n";
    return 0;
  }
  
  //return value
  return this->map[y*Grid::GRID_SIZE + x];
}


bool Grid::thresholdOpenValue(int x, int y){
//TODO: do we need these checks?
//   if ((x<0) || (y<0) || (x>=Grid::GRID_SIZE) || (y>=Grid::GRID_SIZE)) {
//     std::cout << "BAD THRESHOPEN!\n";
//     return false;
//   }
    
  char current = this->map[y*Grid::GRID_SIZE + x];

  if (current <= OPEN_MAX) {
    this->map[y*Grid::GRID_SIZE + x] = current+OPEN;
  } else this->map[y*Grid::GRID_SIZE + x] = Grid::MAX_VALUE;
  
  return (current < THRESHOLD);
}


void Grid::openValue(int x, int y){
  //bounds checking
  if ((x<0) || (y<0) || (x>=Grid::GRID_SIZE) || (y>=Grid::GRID_SIZE)) {
    std::cout << "BAD OPEN!\n";
    return;
  }
  
  char current = this->map[y*Grid::GRID_SIZE + x];
  
  if (current <= OPEN_MAX) {
    this->map[y*Grid::GRID_SIZE + x] = current + OPEN;
  } else this->map[y*Grid::GRID_SIZE + x] = Grid::MAX_VALUE;
}


void Grid::closeValue(int x, int y){
  //bounds checking
  if ((x<0) || (y<0) || (x>=Grid::GRID_SIZE) || (y>=Grid::GRID_SIZE)) {
    std::cout << "BAD CLOSE!\n";
    return;
  }
  
  char current = this->map[y*Grid::GRID_SIZE + x];

  if (current >= CLOSED_MIN) {
    this->map[y*Grid::GRID_SIZE + x] = current + CLOSED;
  } else this->map[y*Grid::GRID_SIZE + x] = -Grid::MAX_VALUE;
}

