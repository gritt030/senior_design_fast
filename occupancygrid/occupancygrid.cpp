
#include "occupancygrid.h"

//gaussian kernels
const int OccupancyGrid::KERNELS[][NUM_KERNELS + 1] = {{13,1,0,0,0,0,0,0,0,0},        //stdev = 0.33 decimeters
                                                       {45,18,1,0,0,0,0,0,0,0},       //stdev = 0.66
                                                       {64,40,10,1,0,0,0,0,0,0},      //stdev = 1
                                                       {74,56,25,7,1,0,0,0,0,0},      //stdev = 1.33
                                                       {79,66,39,16,5,1,0,0,0,0},     //stdev = 1.66
                                                       {82,73,50,27,12,4,1,0,0,0},    //stdev = 2
                                                       {84,77,59,37,20,9,3,1,0,0},    //stdev = 2.33
                                                       {85,80,65,46,28,15,7,3,1,0},   //stdev = 2.66
                                                       {86,82,69,53,36,22,12,6,3,1}}; //stdev = 3
 
const int OccupancyGrid::KERNEL_SUMS[] = {15,83,166,252,333,416,496,575,654};


//constructors
OccupancyGrid::OccupancyGrid(){
  //create initial root node
  this->grid = new Grid();
}

//destructor
OccupancyGrid::~OccupancyGrid(){
  delete this->grid;
}


/*
// original good version
//Bresenham's Algorithm
//make all squares in a node grid between (x1,y1) and (x2,y2) open
// -GRID_SIZE/2 <= x1, x2, y1, y2 <= GRID_SIZE/2
// (0,0) is the central square in the grid
bool OccupancyGrid::openLine(int relX1, int relY1, int relX2, int relY2){
  //information on how we need to move
  int x = relX1;                //position variables
  int y = relY1;
  int signX, signY;             //sign of movement direction
  int deltaX = relX2 - relX1;   //distance we need to move
  int deltaY = relY2 - relY1;
    
  //special cases are handled here
  //single point
  if ((deltaX == 0) && (deltaY == 0)) {
    grid->changeValue(BOUNDARY+x, BOUNDARY-y, OccupancyGrid::OPEN);
    return true;
    
  //vertical line
  } else if (deltaX == 0) {
    signY = deltaY/abs(deltaY);
    
    for (int i=0; i<abs(deltaY); i++) {
      if (grid->getValue(BOUNDARY+x, BOUNDARY-y) < OccupancyGrid::THRESHOLD) {grid->changeValue(BOUNDARY+x, BOUNDARY-y, OccupancyGrid::OPEN); return false;}
      grid->changeValue(BOUNDARY+x, BOUNDARY-y, OccupancyGrid::OPEN);
      y += signY;
    }
    grid->changeValue(BOUNDARY+x, BOUNDARY-y, OccupancyGrid::OPEN);
    return true;
    
  //horizontal line
  } else if (deltaY == 0) {
    int signX = deltaX/abs(deltaX);
    
    for (int i=0; i<abs(deltaX); i++) {
      if (grid->getValue(BOUNDARY+x, BOUNDARY-y) < OccupancyGrid::THRESHOLD) {grid->changeValue(BOUNDARY+x, BOUNDARY-y, OccupancyGrid::OPEN); return false;}
      grid->changeValue(BOUNDARY+x, BOUNDARY-y, OccupancyGrid::OPEN);
      x += signX;
    }
    grid->changeValue(BOUNDARY+x, BOUNDARY-y, OccupancyGrid::OPEN);
    return true;
  }
  
  //switch to absolute value coordinates to make things easier
  signX = deltaX/abs(deltaX);
  signY = deltaY/abs(deltaY);
  deltaX = abs(deltaX);
  deltaY = abs(deltaY);
  
  //SPECIAL CASE: diagonal line
  if (deltaY == deltaX) {
    for (int i=0; i<deltaX; i++) {
      if (grid->getValue(BOUNDARY+x, BOUNDARY-y) < OccupancyGrid::THRESHOLD) {grid->changeValue(BOUNDARY+x, BOUNDARY-y, OccupancyGrid::OPEN); return false;}
      grid->changeValue(BOUNDARY+x, BOUNDARY-y, OccupancyGrid::OPEN);
      x += signX;
      y += signY;
    }
    grid->changeValue(BOUNDARY+x, BOUNDARY-y, OccupancyGrid::OPEN);
    return true;
  }
  
  //change in x and y so far
  int cx=deltaY>>1, cy=deltaX>>1;
  
  //line more horizontal than vertical
  if (deltaX > deltaY) {
    for (int i=0; i<deltaX; i++) {
      //fill in current grid square
      if (grid->getValue(BOUNDARY+x, BOUNDARY-y) < OccupancyGrid::THRESHOLD) {grid->changeValue(BOUNDARY+x, BOUNDARY-y, OccupancyGrid::OPEN); return false;}
      grid->changeValue(BOUNDARY+x, BOUNDARY-y, OccupancyGrid::OPEN);
      
      //see if we need to move vertically
      cy += deltaY;
      if (cy > deltaX) {
        y += signY;
        cy -= deltaX;
        if (grid->getValue(BOUNDARY+x, BOUNDARY-y) < OccupancyGrid::THRESHOLD) {grid->changeValue(BOUNDARY+x, BOUNDARY-y, OccupancyGrid::OPEN); return false;}
        grid->changeValue(BOUNDARY+x, BOUNDARY-y, OccupancyGrid::OPEN);
      }
      
      //move horizontally
      x += signX;
    }
    
  //line more vertical than horizontal
  } else {
    for (int i=0; i<deltaY; i++) {
      //fill in current grid square
      if (grid->getValue(BOUNDARY+x, BOUNDARY-y) < OccupancyGrid::THRESHOLD) {grid->changeValue(BOUNDARY+x, BOUNDARY-y, OccupancyGrid::OPEN); return false;}
      grid->changeValue(BOUNDARY+x, BOUNDARY-y, OccupancyGrid::OPEN);
      
      //see if we need to move horizontally
      cx += deltaX;
      if (cx > deltaY) {
        x += signX;
        cx -= deltaY;
        if (grid->getValue(BOUNDARY+x, BOUNDARY-y) < OccupancyGrid::THRESHOLD) {grid->changeValue(BOUNDARY+x, BOUNDARY-y, OccupancyGrid::OPEN); return false;}
        grid->changeValue(BOUNDARY+x, BOUNDARY-y, OccupancyGrid::OPEN);
      }
      
      //move vertically
      y += signY;
    }
  }
  
  //set last point in line
  grid->changeValue(BOUNDARY+x, BOUNDARY-y, OccupancyGrid::OPEN);
  return true;
}
//*/






// Optimized grid version
bool OccupancyGrid::openLine(int relX1, int relY1, int relX2, int relY2){
  //information on how we need to move
  int x = relX1;                //position variables
  int y = relY1;
  int signX, signY;             //sign of movement direction
  int deltaX = relX2 - relX1;   //distance we need to move
  int deltaY = relY2 - relY1;
    
  //special cases are handled here
  //single point
  if ((deltaX == 0) && (deltaY == 0)) {
    grid->openValue(BOUNDARY+x, BOUNDARY-y);
    return true;
    
  //vertical line
  } else if (deltaX == 0) {
    signY = deltaY/abs(deltaY);
    
    for (int i=0; i<abs(deltaY); i++) {
      if (grid->thresholdOpenValue(BOUNDARY+x, BOUNDARY-y)) return false;
      y += signY;
    }
    grid->openValue(BOUNDARY+x, BOUNDARY-y);
    return true;
    
  //horizontal line
  } else if (deltaY == 0) {
    int signX = deltaX/abs(deltaX);
    
    for (int i=0; i<abs(deltaX); i++) {
      if (grid->thresholdOpenValue(BOUNDARY+x, BOUNDARY-y)) return false;
      x += signX;
    }
    grid->openValue(BOUNDARY+x, BOUNDARY-y);
    return true;
  }
  
  //switch to absolute value coordinates to make things easier
  signX = deltaX/abs(deltaX);
  signY = deltaY/abs(deltaY);
  deltaX = abs(deltaX);
  deltaY = abs(deltaY);
  
  //SPECIAL CASE: diagonal line
  if (deltaY == deltaX) {
    for (int i=0; i<deltaX; i++) {
      if (grid->thresholdOpenValue(BOUNDARY+x, BOUNDARY-y)) return false;
      x += signX;
      y += signY;
    }
    grid->openValue(BOUNDARY+x, BOUNDARY-y);
    return true;
  }
  
  //change in x and y so far
  int cx=deltaY>>1, cy=deltaX>>1;
  
  //line more horizontal than vertical
  if (deltaX > deltaY) {
    for (int i=0; i<deltaX; i++) {
      //fill in current grid square
      if (grid->thresholdOpenValue(BOUNDARY+x, BOUNDARY-y)) return false;
      
      //see if we need to move vertically
      cy += deltaY;
      if (cy > deltaX) {
        y += signY;
        cy -= deltaX;
        if (grid->thresholdOpenValue(BOUNDARY+x, BOUNDARY-y)) return false;
      }
      
      //move horizontally
      x += signX;
    }
    
  //line more vertical than horizontal
  } else {
    for (int i=0; i<deltaY; i++) {
      //fill in current grid square
      if (grid->thresholdOpenValue(BOUNDARY+x, BOUNDARY-y)) return false;
      
      //see if we need to move horizontally
      cx += deltaX;
      if (cx > deltaY) {
        x += signX;
        cx -= deltaY;
        if (grid->thresholdOpenValue(BOUNDARY+x, BOUNDARY-y)) return false;
      }
      
      //move vertically
      y += signY;
    }
  }
  
  //set last point in line
  grid->openValue(BOUNDARY+x, BOUNDARY-y);
  return true;
}//*/















/*
//draw all sonar no matter what. Bad version.
bool OccupancyGrid::openLine(int relX1, int relY1, int relX2, int relY2){
  //information on how we need to move
  int x = relX1;                //position variables
  int y = relY1;
  int signX, signY;             //sign of movement direction
  int deltaX = relX2 - relX1;   //distance we need to move
  int deltaY = relY2 - relY1;
    
  //special cases are handled here
  //single point
  if ((deltaX == 0) && (deltaY == 0)) {
    grid->changeValue(BOUNDARY+x, BOUNDARY-y, OccupancyGrid::OPEN);
    return true;
    
  //vertical line
  } else if (deltaX == 0) {
    signY = deltaY/abs(deltaY);
    
    for (int i=0; i<abs(deltaY); i++) {
      //if (grid->getValue(BOUNDARY+x, BOUNDARY-y) < OccupancyGrid::THRESHOLD) {grid->changeValue(BOUNDARY+x, BOUNDARY-y, OccupancyGrid::OPEN); return false;}
      grid->changeValue(BOUNDARY+x, BOUNDARY-y, OccupancyGrid::OPEN);
      y += signY;
    }
    grid->changeValue(BOUNDARY+x, BOUNDARY-y, OccupancyGrid::OPEN);
    return true;
    
  //horizontal line
  } else if (deltaY == 0) {
    int signX = deltaX/abs(deltaX);
    
    for (int i=0; i<abs(deltaX); i++) {
      //if (grid->getValue(BOUNDARY+x, BOUNDARY-y) < OccupancyGrid::THRESHOLD) {grid->changeValue(BOUNDARY+x, BOUNDARY-y, OccupancyGrid::OPEN); return false;}
      grid->changeValue(BOUNDARY+x, BOUNDARY-y, OccupancyGrid::OPEN);
      x += signX;
    }
    grid->changeValue(BOUNDARY+x, BOUNDARY-y, OccupancyGrid::OPEN);
    return true;
  }
  
  //switch to absolute value coordinates to make things easier
  signX = deltaX/abs(deltaX);
  signY = deltaY/abs(deltaY);
  deltaX = abs(deltaX);
  deltaY = abs(deltaY);
  
  //SPECIAL CASE: diagonal line
  if (deltaY == deltaX) {
    for (int i=0; i<deltaX; i++) {
      //if (grid->getValue(BOUNDARY+x, BOUNDARY-y) < OccupancyGrid::THRESHOLD) {grid->changeValue(BOUNDARY+x, BOUNDARY-y, OccupancyGrid::OPEN); return false;}
      grid->changeValue(BOUNDARY+x, BOUNDARY-y, OccupancyGrid::OPEN);
      x += signX;
      y += signY;
    }
    grid->changeValue(BOUNDARY+x, BOUNDARY-y, OccupancyGrid::OPEN);
    return true;
  }
  
  //change in x and y so far
  int cx=deltaY>>1, cy=deltaX>>1;
  
  //line more horizontal than vertical
  if (deltaX > deltaY) {
    for (int i=0; i<deltaX; i++) {
      //fill in current grid square
      //if (grid->getValue(BOUNDARY+x, BOUNDARY-y) < OccupancyGrid::THRESHOLD) {grid->changeValue(BOUNDARY+x, BOUNDARY-y, OccupancyGrid::OPEN); return false;}
      grid->changeValue(BOUNDARY+x, BOUNDARY-y, OccupancyGrid::OPEN);
      
      //see if we need to move vertically
      cy += deltaY;
      if (cy > deltaX) {
        y += signY;
        cy -= deltaX;
        //if (grid->getValue(BOUNDARY+x, BOUNDARY-y) < OccupancyGrid::THRESHOLD) {grid->changeValue(BOUNDARY+x, BOUNDARY-y, OccupancyGrid::OPEN); return false;}
        grid->changeValue(BOUNDARY+x, BOUNDARY-y, OccupancyGrid::OPEN);
      }
      
      //move horizontally
      x += signX;
    }
    
  //line more vertical than horizontal
  } else {
    for (int i=0; i<deltaY; i++) {
      //fill in current grid square
      //if (grid->getValue(BOUNDARY+x, BOUNDARY-y) < OccupancyGrid::THRESHOLD) {grid->changeValue(BOUNDARY+x, BOUNDARY-y, OccupancyGrid::OPEN); return false;}
      grid->changeValue(BOUNDARY+x, BOUNDARY-y, OccupancyGrid::OPEN);
      
      //see if we need to move horizontally
      cx += deltaX;
      if (cx > deltaY) {
        x += signX;
        cx -= deltaY;
        //if (grid->getValue(BOUNDARY+x, BOUNDARY-y) < OccupancyGrid::THRESHOLD) {grid->changeValue(BOUNDARY+x, BOUNDARY-y, OccupancyGrid::OPEN); return false;}
        grid->changeValue(BOUNDARY+x, BOUNDARY-y, OccupancyGrid::OPEN);
      }
      
      //move vertically
      y += signY;
    }
  }
  
  //set last point in line
  grid->changeValue(BOUNDARY+x, BOUNDARY-y, OccupancyGrid::OPEN);
  return true;
}
//*/
















//make all squares in a node grid between (x1,y1) and (x2,y2) open and close point (x2,y2)
// -GRID_SIZE/2 <= x1, x2, y1, y2 <= GRID_SIZE/2
// (0,0) is the central square in the grid
void OccupancyGrid::closeLine(int relX1, int relY1, int relX2, int relY2){
  //open line in grid
  if (this->openLine(relX1, relY1, relX2, relY2)) {
    //set final point of line to closed
    grid->changeValue(BOUNDARY+relX2, BOUNDARY-relY2, -OccupancyGrid::OPEN);
    grid->closeValue(BOUNDARY+relX2, BOUNDARY-relY2);
  }
}


void OccupancyGrid::sendToImage(char* filename, int x, int y){
  PPMwriter* w = new PPMwriter();
  int size = Grid::GRID_SIZE;
  
  w->create_image(filename, size, size);
  
  for (int i=0; i<Grid::GRID_SIZE; i++){
    for (int j=0; j<Grid::GRID_SIZE; j++) {
      char cur = grid->map[i*Grid::GRID_SIZE + j];
      setImagePixel(w, cur);
    }
  }
  
  w->output_image();
  delete w;
}


void OccupancyGrid::setImagePixel(PPMwriter* w, char value){  
  //open map square
  if (value > 0) {
    w->write_pixel(0x00, value, 0xff);
  
  //closed map square
  } else if (value < 0) {
    w->write_pixel(0xff, value, 0x00);
  
  //unknown map square
  } else {
    w->write_pixel(0x80, 0x80, 0x80);
  }
}


void OccupancyGrid::openSlice(int relX1, int relY1, int relX2, int relY2, float angle){
  //information on how we need to move
  int deltaX = relX2 - relX1;   //distance we need to move
  int deltaY = relY2 - relY1;
  int dist = (int)round(sqrt(deltaX*deltaX + deltaY*deltaY)*tan(angle));  //perpendicular distance along one leg
  
  //special cases are handled here
  //single point
  if ((deltaX == 0) && (deltaY == 0)) {
    this->openLine(relX1, relY1, relX2, relY2);
    return;
    
  //vertical line passed in
  } else if (deltaX == 0) {
    for (int i=-dist; i<=dist; i++) {
      this->openLine(relX1, relY1, relX2+i, relY2);
    }
    return;
    
  //horizontal line passed in
  } else if (deltaY == 0) {
    for (int i=-dist; i<=dist; i++) {
      this->openLine(relX1, relY1, relX2, relY2+i);
    }
    return;
  }
  
  //get slope we're moving at
  float slope = -(float)deltaX/(float)deltaY;

  //for block size
  deltaX = abs(deltaX);
  deltaY = abs(deltaY);
  
  //amount we need to change x and y by to get ends of line
  int changeX = (int)round(sqrt((float)(dist*dist) / (1.0+slope*slope)));
  int changeY = (int)round(sqrt((float)(dist*dist) / (1.0+1.0/(slope*slope))) * (slope/fabs(slope)));
  
  //starting and ending points of line
  int startX = relX2 - changeX;
  int startY = relY2 - changeY;
  int x = startX;
  int y = startY;
  
  //used to determine how to move
  int ySign;
  int block;
  
  //how many times we perform the loop for
  int loop;
  if (deltaX > deltaY){
    loop = abs(changeY)<<1;
    block = deltaX;
  }
  else {
    loop = changeX<<1;
    block = deltaY;
  }
  
  if (slope < 0) ySign = -1;
  else ySign = 1;
  
  //keep track of pixel transitions
  int cx = block>>1;
  int cy = block>>1;
    
  this->openLine(relX1, relY1, x, y);
  
  do {
    cx += deltaY;
    cy += deltaX;
    if (cx > block) {
      x++;
      cx -= block;
    }
    
    if (cy > block) {
      y += ySign;
      cy -= block;
    }
    
    //std::cout <<x<<","<<y<<std::endl;
    this->openLine(relX1, relY1, x, y);
  } while (loop-- > 0);
}


void OccupancyGrid::closeSlice(int relX1, int relY1, int relX2, int relY2, float angle){
  //information on how we need to move
  int deltaX = relX2 - relX1;   //distance we need to move
  int deltaY = relY2 - relY1;
  int dist = (int)round(sqrt(deltaX*deltaX + deltaY*deltaY)*tan(angle));  //perpendicular distance along one leg
  
  //special cases are handled here
  //single point
  if ((deltaX == 0) && (deltaY == 0)) {
    this->closeLine(relX1, relY1, relX2, relY2);
    return;
    
  //vertical line passed in
  } else if (deltaX == 0) {
    for (int i=-dist; i<=dist; i++) {
      this->closeLine(relX1, relY1, relX2+i, relY2);
    }
    return;
    
  //horizontal line passed in
  } else if (deltaY == 0) {
    for (int i=-dist; i<=dist; i++) {
      this->closeLine(relX1, relY1, relX2, relY2+i);
    }
    return;
  }
  
  //get slope we're moving at
  float slope = -(float)deltaX/(float)deltaY;

  //for block size
  deltaX = abs(deltaX);
  deltaY = abs(deltaY);
  
  //amount we need to change x and y by to get ends of line
  int changeX = (int)round(sqrt((float)(dist*dist) / (1.0+slope*slope)));
  int changeY = (int)round(sqrt((float)(dist*dist) / (1.0+1.0/(slope*slope))) * (slope/fabs(slope)));
  
  //starting and ending points of line
  int startX = relX2 - changeX;
  int startY = relY2 - changeY;
  int x = startX;
  int y = startY;
  
  //used to determine how to move
  int ySign;
  int block;
  
  //how many times we perform the loop for
  int loop;
  if (deltaX > deltaY){
    loop = abs(changeY)<<1;
    block = deltaX;
  }
  else {
    loop = changeX<<1;
    block = deltaY;
  }
  
  if (slope < 0) ySign = -1;
  else ySign = 1;
  
  //keep track of pixel transitions
  int cx = block>>1;
  int cy = block>>1;
    
  this->closeLine(relX1, relY1, x, y);
  
  do {
    cx += deltaY;
    cy += deltaX;
    if (cx > block) {
      x++;
      cx -= block;
    }
    
    if (cy > block) {
      y += ySign;
      cy -= block;
    }
    
    //std::cout <<x<<","<<y<<std::endl;
    this->closeLine(relX1, relY1, x, y);
  } while (loop-- > 0);
}


//blur the map in the x direction
void OccupancyGrid::blurMapX(int uncertainty){
  //TODO:make sure uncertainty is not too small/large
//   if (uncertainty < 1) return;
//  
//   //uncertainty is too large, iterate for equivalent kernel
//   else if (uncertainty > this->NUM_KERNELS) {
//     int unc2 = uncertainty*uncertainty;
//     int num2 = this->NUM_KERNELS * this->NUM_KERNELS;
//     int blur2 = (int)(round(sqrt(unc2 - num2)));
//     
//     this->blurMapX(this->NUM_KERNELS);
//     this->blurMapX(blur2);
//     return;
//   }
  
  const int* kernel = this->KERNELS[uncertainty-1];
  const int kernelSum = this->KERNEL_SUMS[uncertainty-1];
  char* buffer = new char[uncertainty];
  
  for (int i=0; i<Grid::GRID_SIZE; i++){
    for (int j=0; j<Grid::GRID_SIZE+uncertainty; j++){
      //calculate current kernel sum
      int sum = 0;
      for (int k=uncertainty; k>0; k--){
        sum += (int)(grid->getValue(j-k,i)) * kernel[k];
        sum += (int)(grid->getValue(j+k,i)) * kernel[k];
      }
      sum += (int)(grid->getValue(j,i)) * kernel[0];
      sum /= kernelSum;
      
      //set unneeded values to new values
      grid->setValue(j-uncertainty,i,buffer[0]);
      
      //update buffer
      for (int k=1; k<uncertainty; k++){
        buffer[k-1] = buffer[k];
      }
      buffer[uncertainty-1] = (char)sum;
    }
  }
  delete[] buffer;
}


//blur the map in the y direction
void OccupancyGrid::blurMapY(int uncertainty){
  //TODO:make sure uncertainty is not too small/large
//   if (uncertainty < 1) return;
//   
//   //uncertainty is too large, iterate for equivalent kernel
//   else if (uncertainty > this->NUM_KERNELS) {
//     int unc2 = uncertainty*uncertainty;
//     int num2 = this->NUM_KERNELS * this->NUM_KERNELS;
//     int blur2 = (int)(round(sqrt(unc2 - num2)));
//     
//     this->blurMapY(this->NUM_KERNELS);
//     this->blurMapY(blur2);
//     return;
//   }
  
  const int* kernel = this->KERNELS[uncertainty-1];
  const int kernelSum = this->KERNEL_SUMS[uncertainty-1];
  char* buffer = new char[uncertainty];
  
  for (int i=0; i<Grid::GRID_SIZE; i++){
    for (int j=0; j<Grid::GRID_SIZE+uncertainty; j++){
      //calculate current kernel sum
      int sum = 0;
      for (int k=uncertainty; k>0; k--){
        sum += (int)(grid->getValue(i,j-k)) * kernel[k];
        sum += (int)(grid->getValue(i,j+k)) * kernel[k];
      }
      sum += (int)(grid->getValue(i,j)) * kernel[0];
      sum /= kernelSum;
      
      //set unneeded values to new values
      grid->setValue(i,j-uncertainty,buffer[0]);
      
      //update buffer
      for (int k=1; k<uncertainty; k++){
        buffer[k-1] = buffer[k];
      }
      buffer[uncertainty-1] = (char)sum;
    }
  }
  delete[] buffer;
}



//merge the given grid with this one
void OccupancyGrid::mergeMaps(OccupancyGrid* newGrid){
  for(int i=0; i<Grid::GRID_SIZE; i++){
    for(int j=0; j<Grid::GRID_SIZE; j++){
      char cur = newGrid->grid->map[i*Grid::GRID_SIZE + j];
      this->grid->changeValue(j,i,cur);
    }
  }
}



//overlay the given grid onto this one
void OccupancyGrid::overlayMaps(OccupancyGrid* newGrid){
  for(int i=0; i<Grid::GRID_SIZE; i++){
    for(int j=0; j<Grid::GRID_SIZE; j++){
      char cur = newGrid->grid->map[i*Grid::GRID_SIZE + j];
      if (cur != 0) this->grid->map[i*Grid::GRID_SIZE + j] = cur;
    }
  }
}



//copy just the walls of the occupancy grid into newGrid
void OccupancyGrid::getWallMap(OccupancyGrid* newGrid){
  for(int i=0; i<Grid::GRID_SIZE; i++){
    for(int j=0; j<Grid::GRID_SIZE; j++){
      char cur = this->grid->map[i*Grid::GRID_SIZE + j];
      if (cur < 0) newGrid->grid->map[i*Grid::GRID_SIZE + j] = cur;
    }
  }
}


int OccupancyGrid::getWallMap(std::vector<int>* xpos, std::vector<int>* ypos){
  int count = 0;
  
  for(int i=0; i<Grid::GRID_SIZE; i++){
    for(int j=0; j<Grid::GRID_SIZE; j++){
      char cur = this->grid->map[i*Grid::GRID_SIZE + j];
      if (cur < 0){
        xpos->push_back(j);
        ypos->push_back(i);
        count++;
      }
    }
  }
  
  xpos->shrink_to_fit();
  ypos->shrink_to_fit();
  return count;
}


//copy just the open areas of the occupancy grid into newGrid
void OccupancyGrid::getOpenMap(OccupancyGrid* newGrid){
  for(int i=0; i<Grid::GRID_SIZE; i++){
    for(int j=0; j<Grid::GRID_SIZE; j++){
      char cur = this->grid->map[i*Grid::GRID_SIZE + j];
      if (cur > 0) newGrid->grid->map[i*Grid::GRID_SIZE + j] = cur;
    }
  }
}

