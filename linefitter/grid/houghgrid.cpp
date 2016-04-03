
#include "houghgrid.h"

//constructor, create grid
HoughGrid::HoughGrid() {
  this->map = new unsigned char[RADIUS_SIZE * THETA_SIZE]();
  this->D_THETA = 3.141592654 / THETA_SIZE;
}

//destructor, destroy grid
HoughGrid::~HoughGrid() {
  delete[] this->map;
}


//add the hough transform of a point into the hough grid
void HoughGrid::addHoughPoint(int x, int y){
  double theta = 0.0;
  Num_Points++;
  int dx = x-CENTER;
  int dy = y-CENTER;
  int rad;
  
  for (int i=0; i<THETA_SIZE; i++) {
    rad = (int)(dx*cos(theta) + dy*sin(theta));
    rad += ADDITION;
         
    this->map[i*RADIUS_SIZE + (rad >> 3)] |= (0x01 << (rad % 8));
    
    theta += D_THETA;
  }
}


//sum the number of filled cells for each theta column
int HoughGrid::getThetaSums(int* sums){
  int curSum;
  unsigned char curByte;
  
  
  for (int i=0; i<THETA_SIZE; i++){
    curSum = 0;
    
    for (int j=0; j<RADIUS_SIZE; j++){
      curByte = this->map[i*RADIUS_SIZE + j];
      
      curSum += curByte & 0x01;
      curSum += (curByte >> 1) & 0x01;
      curSum += (curByte >> 2) & 0x01;
      curSum += (curByte >> 3) & 0x01;
      curSum += (curByte >> 4) & 0x01;
      curSum += (curByte >> 5) & 0x01;
      curSum += (curByte >> 6) & 0x01;
      curSum += (curByte >> 7) & 0x01;
    }
    
    sums[i] = curSum;
    //std::cout << i << ", " << curSum << std::endl;
  }
  
  return this->Num_Points;
}




void HoughGrid::sendHoughToImage(char* filename){
  PngWriter* w = new PngWriter();
  unsigned char curByte;
  
  w->create_image(filename, THETA_SIZE, RADIUS_SIZE*8);
  
  for (int i=0; i<THETA_SIZE; i++){
    
    for (int j=0; j<RADIUS_SIZE; j++){
      curByte = this->map[i*RADIUS_SIZE + j];

      setImagePixel(w, i, j*8, curByte & 0x01);
      setImagePixel(w, i, j*8+1, (curByte >> 1) & 0x01);
      setImagePixel(w, i, j*8+2, (curByte >> 2) & 0x01);
      setImagePixel(w, i, j*8+3, (curByte >> 3) & 0x01);
      setImagePixel(w, i, j*8+4, (curByte >> 4) & 0x01);
      setImagePixel(w, i, j*8+5, (curByte >> 5) & 0x01);
      setImagePixel(w, i, j*8+6, (curByte >> 6) & 0x01);
      setImagePixel(w, i, j*8+7, (curByte >> 7) & 0x01);
    }
  }
  
  w->output_image();
  delete w;
}


void HoughGrid::setImagePixel(PngWriter* w, int x, int y, int val){
  int color;
  
  //open map square
  if (val > 0) {
    color = 0x008800ff;
  
  //unknown map square
  } else {
    color = 0x808080ff;
  }
  
  w->set_pixel(x, y, color);
}

