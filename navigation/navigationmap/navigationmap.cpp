
#include "navigationmap.h"

//constructors
NavigationMap::NavigationMap(){
  //create initial root node
  this->grid = new OpenGrid();
  this->frontiers = new FrontierGrid();
}

//destructor
NavigationMap::~NavigationMap(){
  delete this->grid;
  delete this->frontiers;
}


//Bresenham's Algorithm
//make all squares in a node grid between (x1,y1) and (x2,y2) open
// -GRID_SIZE/2 <= x1, x2, y1, y2 <= GRID_SIZE/2
// (0,0) is the central square in the grid
void NavigationMap::openLine(int relX1, int relY1, int relX2, int relY2){
  //information on how we need to move
  int x = relX1;                //position variables
  int y = relY1;
  int signX, signY;             //sign of movement direction
  int deltaX = relX2 - relX1;   //distance we need to move
  int deltaY = relY2 - relY1;
  
  //special cases are handled here
  //single point
  if ((deltaX == 0) && (deltaY == 0)) {
    grid->changeValue(x, y, OPEN);
    return;
    
  //vertical line
  } else if (deltaX == 0) {
    signY = deltaY/abs(deltaY);
    
    for (int i=0; i<abs(deltaY); i++) {
      grid->changeValue(x, y, OPEN);
      y += signY;
    }
    grid->changeValue(x, y, OPEN);
    return;
    
  //horizontal line
  } else if (deltaY == 0) {
    int signX = deltaX/abs(deltaX);
    
    for (int i=0; i<abs(deltaX); i++) {
      grid->changeValue(x, y, OPEN);
      x += signX;
    }
    grid->changeValue(x, y, OPEN);
    return;
  }
  
  //switch to absolute value coordinates to make things easier
  signX = deltaX/abs(deltaX);
  signY = deltaY/abs(deltaY);
  deltaX = abs(deltaX);
  deltaY = abs(deltaY);
  
  //SPECIAL CASE: diagonal line
  if (deltaY == deltaX) {
    for (int i=0; i<deltaX; i++) {
      grid->changeValue(x, y, OPEN);
      x += signX;
      y += signY;
    }
    grid->changeValue(x, y, OPEN);
    return;
  }
  
  //change in x and y so far
  int cx=deltaY>>1, cy=deltaX>>1;
  
  //line more horizontal than vertical
  if (deltaX > deltaY) {
    for (int i=0; i<deltaX; i++) {
      //fill in current grid square
      grid->changeValue(x, y, OPEN);
      
      //see if we need to move vertically
      cy += deltaY;
      if (cy > deltaX) {
        y += signY;
        cy -= deltaX;
        grid->changeValue(x, y, OPEN);
      }
      
      //move horizontally
      x += signX;
    }
    
  //line more vertical than horizontal
  } else {
    for (int i=0; i<deltaY; i++) {
      //fill in current grid square
      grid->changeValue(x, y, OPEN);
      
      //see if we need to move horizontally
      cx += deltaX;
      if (cx > deltaY) {
        x += signX;
        cx -= deltaY;
        grid->changeValue(x, y, OPEN);
      }
      
      //move vertically
      y += signY;
    }
  }
  
  //set last point in line
  grid->changeValue(x, y, OPEN);
  return;
} //*/


void NavigationMap::openFrontierLine(int relX1, int relY1, int relX2, int relY2){
  //information on how we need to move
  int x = relX1;                //position variables
  int y = relY1;
  int signX, signY;             //sign of movement direction
  int deltaX = relX2 - relX1;   //distance we need to move
  int deltaY = relY2 - relY1;
  
  //special cases are handled here
  //single point
  if ((deltaX == 0) && (deltaY == 0)) {
    grid->changeValue(x, y, OPEN);
    frontiers->changeValue(x, y, FRONTIER);
    return;
    
  //vertical line
  } else if (deltaX == 0) {
    signY = deltaY/abs(deltaY);
    
    for (int i=0; i<abs(deltaY); i++) {
      grid->changeValue(x, y, OPEN);
      frontiers->changeValue(x, y, FRONTIER);
      y += signY;
    }
    grid->changeValue(x, y, OPEN);
    frontiers->changeValue(x, y, FRONTIER);
    return;
    
  //horizontal line
  } else if (deltaY == 0) {
    int signX = deltaX/abs(deltaX);
    
    for (int i=0; i<abs(deltaX); i++) {
      grid->changeValue(x, y, OPEN);
      frontiers->changeValue(x, y, FRONTIER);
      x += signX;
    }
    grid->changeValue(x, y, OPEN);
    frontiers->changeValue(x, y, FRONTIER);
    return;
  }
  
  //switch to absolute value coordinates to make things easier
  signX = deltaX/abs(deltaX);
  signY = deltaY/abs(deltaY);
  deltaX = abs(deltaX);
  deltaY = abs(deltaY);
  
  //SPECIAL CASE: diagonal line
  if (deltaY == deltaX) {
    for (int i=0; i<deltaX; i++) {
      grid->changeValue(x, y, OPEN);
      frontiers->changeValue(x, y, FRONTIER);
      x += signX;
      y += signY;
    }
    grid->changeValue(x, y, OPEN);
    frontiers->changeValue(x, y, FRONTIER);
    return;
  }
  
  //change in x and y so far
  int cx=deltaY>>1, cy=deltaX>>1;
  
  //line more horizontal than vertical
  if (deltaX > deltaY) {
    for (int i=0; i<deltaX; i++) {
      //fill in current grid square
      grid->changeValue(x, y, OPEN);
      frontiers->changeValue(x, y, FRONTIER);
      
      //see if we need to move vertically
      cy += deltaY;
      if (cy > deltaX) {
        y += signY;
        cy -= deltaX;
        grid->changeValue(x, y, OPEN);
        frontiers->changeValue(x, y, FRONTIER);
      }
      
      //move horizontally
      x += signX;
    }
    
  //line more vertical than horizontal
  } else {
    for (int i=0; i<deltaY; i++) {
      //fill in current grid square
      grid->changeValue(x, y, OPEN);
      frontiers->changeValue(x, y, FRONTIER);
      
      //see if we need to move horizontally
      cx += deltaX;
      if (cx > deltaY) {
        x += signX;
        cx -= deltaY;
        grid->changeValue(x, y, OPEN);
        frontiers->changeValue(x, y, FRONTIER);
      }
      
      //move vertically
      y += signY;
    }
  }
  
  //set last point in line
  grid->changeValue(x, y, OPEN);
  frontiers->changeValue(x, y, FRONTIER);
  return;
} //*/


//make all squares in a node grid between (x1,y1) and (x2,y2) open
// -GRID_SIZE/2 <= x1, x2, y1, y2 <= GRID_SIZE/2
// (0,0) is the central square in the grid
void NavigationMap::openLineSide(int relX1, int relY1, int relX2, int relY2){
  //open line in grid
  this->openLine(relX1, relY1, relX2, relY2);
    frontiers->changeValue(relX2, relY2, FRONTIER);
}


//make all squares in a node grid between (x1,y1) and (x2,y2) open
// -GRID_SIZE/2 <= x1, x2, y1, y2 <= GRID_SIZE/2
// (0,0) is the central square in the grid
void NavigationMap::openLineFront(int relX1, int relY1, int relX2, int relY2){
  //open line in grid
  this->openLine(relX1, relY1, relX2, relY2);
    this->frontiers->changeValue(relX2, relY2, UNFRONTIER);
}


//make all squares in a node grid between (x1,y1) and (x2,y2) open and close point (x2,y2)
// -GRID_SIZE/2 <= x1, x2, y1, y2 <= GRID_SIZE/2
// (0,0) is the central square in the grid
void NavigationMap::closeLineSide(int relX1, int relY1, int relX2, int relY2){
  //open line in grid
  this->openLine(relX1, relY1, relX2, relY2);
    //set final point of line to closed
    grid->changeValue(relX2, relY2, -OPEN);
    grid->changeValue(relX2, relY2, CLOSED);
    frontiers->changeValue(relX2, relY2, UNFRONTIER);
}


void NavigationMap::closeFrontierLine(int relX1, int relY1, int relX2, int relY2){
  //open line in grid
  this->openFrontierLine(relX1, relY1, relX2, relY2);
    //set final point of line to closed
    grid->changeValue(relX2, relY2, -OPEN);
    grid->changeValue(relX2, relY2, CLOSED);
    frontiers->changeValue(relX2, relY2, UNFRONTIER);
}


//make all squares in a node grid between (x1,y1) and (x2,y2) open and close point (x2,y2)
// -GRID_SIZE/2 <= x1, x2, y1, y2 <= GRID_SIZE/2
// (0,0) is the central square in the grid
void NavigationMap::closeLineFront(int relX1, int relY1, int relX2, int relY2){
  //open line in grid
  this->openLine(relX1, relY1, relX2, relY2);
    //set final point of line to closed
    grid->changeValue(relX2, relY2, -OPEN);
    grid->changeValue(relX2, relY2, CLOSED);
    frontiers->changeValue(relX2, relY2, UNFRONTIER);
}


void NavigationMap::sendToImage(char* filename){
  PngWriter* w = new PngWriter();
  int size = OpenGrid::GRID_SIZE;
  
  w->create_image(filename, size, size);
  
  for (int i=0; i<OpenGrid::GRID_SIZE; i++){
    for (int j=0; j<OpenGrid::GRID_SIZE; j++) {
      unsigned short cur = grid->getValue(i-OpenGrid::GRID_BOUNDARY, OpenGrid::GRID_BOUNDARY-j);
      setMapImagePixel(w, i, j, cur);
    }
  }
  
  for (int i=0; i<OpenGrid::GRID_SIZE; i++){
    for (int j=0; j<OpenGrid::GRID_SIZE; j++) {
      unsigned char cur = frontiers->getValue(i-OpenGrid::GRID_BOUNDARY, OpenGrid::GRID_BOUNDARY-j);
      if (cur != 0) setFrontierImagePixel(w, i, j, cur);
    }
  }
  
  w->output_image();
  delete w;
}


void NavigationMap::sendMapToImage(char* filename){
  PngWriter* w = new PngWriter();
  int size = OpenGrid::GRID_SIZE;
  
  w->create_image(filename, size, size);
  
  for (int i=0; i<OpenGrid::GRID_SIZE; i++){
    for (int j=0; j<OpenGrid::GRID_SIZE; j++) {
      unsigned short cur = grid->getValue(i-OpenGrid::GRID_BOUNDARY, OpenGrid::GRID_BOUNDARY-j);
      setMapImagePixel(w, i, j, cur);
    }
  }
  
  w->output_image();
  delete w;
}


void NavigationMap::sendFrontierToImage(char* filename){
  PngWriter* w = new PngWriter();
  int size = OpenGrid::GRID_SIZE;
  
  w->create_image(filename, size, size);
  
  for (int i=0; i<OpenGrid::GRID_SIZE; i++){
    for (int j=0; j<OpenGrid::GRID_SIZE; j++) {
      unsigned char cur = frontiers->getValue(i-OpenGrid::GRID_BOUNDARY, OpenGrid::GRID_BOUNDARY-j);
      setFrontierImagePixel(w, i, j, cur);
    }
  }
  
  w->output_image();
  delete w;
}


void NavigationMap::setMapImagePixel(PngWriter* w, int x, int y, unsigned short value){
  int color;
  
  //square highly opened
  if (value > 32767) {
    value -= 32768;
    char blue = (char)((32767.0f-(float)value) / 128.0f);
    color = 0x00ff00;
    color |= (blue & 0xff);
    color <<= 8;
    color |= 0xff;
  
  //square kind of opened
  } else if (value > 0) {
    char green = (char)((float)value / 128.0f);
    color = (green & 0xff);
    color <<= 16;
    color |= 0xffff;
  
  //no information
  } else {
    color = 0x808080ff;
  }
  
  w->set_pixel(x, y, color);
}


void NavigationMap::setFrontierImagePixel(PngWriter* w, int x, int y, unsigned char value){
  int color;
  
  //square frontiered
  if (value > 0) {
    color = (255-value) & 0xff;
    color <<= 16;
    color |= 0xff0000ff;
  
  //no information
  } else {
    color = 0x808080ff;
  }
  
  w->set_pixel(x, y, color);
}


void NavigationMap::openSliceSide(int relX1, int relY1, int relX2, int relY2, float angle){
  //information on how we need to move
  int deltaX = relX2 - relX1;   //distance we need to move
  int deltaY = relY2 - relY1;
  int dist = (int)round(sqrt(deltaX*deltaX + deltaY*deltaY)*tan(angle));  //perpendicular distance along one leg
  
  //special cases are handled here
  //single point
  if ((deltaX == 0) && (deltaY == 0)) {
    this->openLineSide(relX1, relY1, relX2, relY2);
    return;
    
  //vertical line passed in
  } else if (deltaX == 0) {
    this->openFrontierLine(relX1, relY1, relX2-dist, relY2);
    for (int i=(1-dist); i<dist; i++) {
      this->openLineSide(relX1, relY1, relX2+i, relY2);
    }
    this->openFrontierLine(relX1, relY1, relX2+dist, relY2);
    return;
    
  //horizontal line passed in
  } else if (deltaY == 0) {
    this->openFrontierLine(relX1, relY1, relX2, relY2-dist);
    for (int i=(1-dist); i<dist; i++) {
      this->openLineSide(relX1, relY1, relX2, relY2+i);
    }
    this->openFrontierLine(relX1, relY1, relX2, relY2+dist);
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
  
  //originally openLineSide if this breaks
  this->openFrontierLine(relX1, relY1, x, y);
  
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
    
    this->openLineSide(relX1, relY1, x, y);
  } while (loop-- > 1); //originally 0
  
  //originally not here (rest of function)
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
  
  this->openFrontierLine(relX1, relY1, x, y);
}


void NavigationMap::openSliceFront(int relX1, int relY1, int relX2, int relY2, float angle){
  //information on how we need to move
  int deltaX = relX2 - relX1;   //distance we need to move
  int deltaY = relY2 - relY1;
  int dist = (int)round(sqrt(deltaX*deltaX + deltaY*deltaY)*tan(angle));  //perpendicular distance along one leg
  
  //special cases are handled here
  //single point
  if ((deltaX == 0) && (deltaY == 0)) {
    this->openLineFront(relX1, relY1, relX2, relY2);
    return;
    
  //vertical line passed in
  } else if (deltaX == 0) {
    for (int i=-dist; i<=dist; i++) {
      this->openLineFront(relX1, relY1, relX2+i, relY2);
    }
    return;
    
  //horizontal line passed in
  } else if (deltaY == 0) {
    for (int i=-dist; i<=dist; i++) {
      this->openLineFront(relX1, relY1, relX2, relY2+i);
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
    
  this->openLineFront(relX1, relY1, x, y);
  
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
    
    this->openLineFront(relX1, relY1, x, y);
  } while (loop-- > 0);
}


void NavigationMap::closeSliceSide(int relX1, int relY1, int relX2, int relY2, float angle){
  //information on how we need to move
  int deltaX = relX2 - relX1;   //distance we need to move
  int deltaY = relY2 - relY1;
  int dist = (int)round(sqrt(deltaX*deltaX + deltaY*deltaY)*tan(angle));  //perpendicular distance along one leg
  
  //special cases are handled here
  //single point
  if ((deltaX == 0) && (deltaY == 0)) {
    this->closeLineSide(relX1, relY1, relX2, relY2);
    return;
    
  //vertical line passed in
  } else if (deltaX == 0) {
    this->closeFrontierLine(relX1, relY1, relX2-dist, relY2);
    for (int i=(1-dist); i<dist; i++) {
      this->closeLineSide(relX1, relY1, relX2+i, relY2);
    }
    this->closeFrontierLine(relX1, relY1, relX2+dist, relY2);
    return;
    
  //horizontal line passed in
  } else if (deltaY == 0) {
    this->closeFrontierLine(relX1, relY1, relX2, relY2-dist);
    for (int i=(1-dist); i<dist; i++) {
      this->closeLineSide(relX1, relY1, relX2, relY2+i);
    }
    this->closeFrontierLine(relX1, relY1, relX2, relY2+dist);
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
  
  //originally openLineSide if this breaks
  this->closeFrontierLine(relX1, relY1, x, y);
  
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
    
    this->closeLineSide(relX1, relY1, x, y);
  } while (loop-- > 1); //originally 0
  
  //originally not here (rest of function)
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
  
  this->closeFrontierLine(relX1, relY1, x, y);
}


void NavigationMap::closeSliceFront(int relX1, int relY1, int relX2, int relY2, float angle){
  //information on how we need to move
  int deltaX = relX2 - relX1;   //distance we need to move
  int deltaY = relY2 - relY1;
  int dist = (int)round(sqrt(deltaX*deltaX + deltaY*deltaY)*tan(angle));  //perpendicular distance along one leg
  
  //special cases are handled here
  //single point
  if ((deltaX == 0) && (deltaY == 0)) {
    this->closeLineFront(relX1, relY1, relX2, relY2);
    return;
    
  //vertical line passed in
  } else if (deltaX == 0) {
    for (int i=-dist; i<=dist; i++) {
      this->closeLineFront(relX1, relY1, relX2+i, relY2);
    }
    return;
    
  //horizontal line passed in
  } else if (deltaY == 0) {
    for (int i=-dist; i<=dist; i++) {
      this->closeLineFront(relX1, relY1, relX2, relY2+i);
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
    
  this->closeLineFront(relX1, relY1, x, y);
  
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
    
    this->closeLineFront(relX1, relY1, x, y);
  } while (loop-- > 0);
}





void NavigationMap::cleanFrontier(){
  FrontierGrid *newGrid = new FrontierGrid();
  
  //clear frontiers that are surrounded by closed cells
  for (int i=-OpenGrid::GRID_BOUNDARY; i<=OpenGrid::GRID_BOUNDARY; i++){
    for (int j=-OpenGrid::GRID_BOUNDARY; j<=OpenGrid::GRID_BOUNDARY; j++) {
      if (frontiers->getValue(i,j) > THRESHOLD){
        int emptyMap = 0;
        if (grid->getValue(i+1,j) == 0) emptyMap++;
        if (grid->getValue(i-1,j) == 0) emptyMap++;
        if (grid->getValue(i,j+1) == 0) emptyMap++;
        if (grid->getValue(i,j-1) == 0) emptyMap++;
        
        if (emptyMap != 0)
          newGrid->setValue(i,j,frontiers->getValue(i,j));
      }
    }
  }
  
  delete this->frontiers;
  this->frontiers = newGrid;
  newGrid = new FrontierGrid();
  
  //clear frontiers that are alone
  for (int i=-OpenGrid::GRID_BOUNDARY; i<=OpenGrid::GRID_BOUNDARY; i++){
    for (int j=-OpenGrid::GRID_BOUNDARY; j<=OpenGrid::GRID_BOUNDARY; j++) {
      if (frontiers->getValue(i,j) > THRESHOLD){
        int fullFron = 0;
        if (frontiers->getValue(i+1,j) != 0) fullFron++;
        if (frontiers->getValue(i-1,j) != 0) fullFron++;
        if (frontiers->getValue(i,j+1) != 0) fullFron++;
        if (frontiers->getValue(i,j-1) != 0) fullFron++;
        
        if (fullFron != 0)
          newGrid->setValue(i,j,frontiers->getValue(i,j));
      }
    }
  }
  
  delete this->frontiers;
  this->frontiers = newGrid;
}

