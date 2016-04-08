
#include "houghgrid.h"

//constructor, create grid
HoughGrid::HoughGrid() {
  this->map = new unsigned short[RADIUS_SIZE * THETA_SIZE]();
  this->D_THETA = 3.141592654 / THETA_SIZE;
  
  SIN_ARRAY = new float[THETA_SIZE];
  COS_ARRAY = new float[THETA_SIZE];
  
  float theta = 0.0;
  for (int i=0; i<THETA_SIZE; i++){
    SIN_ARRAY[i] = (float)sin(theta);
    COS_ARRAY[i] = (float)cos(theta);
    theta += D_THETA;
  }
}

//destructor, destroy grid
HoughGrid::~HoughGrid() {
  delete[] this->map;
  delete[] this->SIN_ARRAY;
  delete[] this->COS_ARRAY;
}


//add the hough transform of a point into the hough grid
void HoughGrid::addHoughPoint(int x, int y){
  this->Num_Points++;
  int dx = x-CENTER;
  int dy = y-CENTER;
  int rad;
  
  for (int i=0; i<THETA_SIZE; i++) {
    rad = (int)(dx*COS_ARRAY[i] + dy*SIN_ARRAY[i]);
    rad >>= OFFSET;
    rad += ADDITION;
    
    this->map[i*RADIUS_SIZE + rad]++;   
  }
}


//get the highest peak for each theta column
int HoughGrid::getThetaPeaks(int* sums){
  unsigned short curMax;
  unsigned short curVal;
  
  for (int i=0; i<THETA_SIZE; i++){
    curMax = 0;
    
    for (int j=0; j<RADIUS_SIZE; j++){
      curVal = this->map[i*RADIUS_SIZE + j];
      if (curVal > curMax) curMax = curVal;
    }
    
    sums[i] = (int)curMax;
    //std::cout << i << ", " << (int)curMax << std::endl;
  }
  
  return this->Num_Points;
}



//write the hough transform to an image
void HoughGrid::sendHoughToImage(char* filename){
  PPMwriter* w = new PPMwriter();
  unsigned short curVal;
  
  w->create_image(filename, RADIUS_SIZE, THETA_SIZE);
  
  for (int i=0; i<THETA_SIZE; i++){
    for (int j=0; j<RADIUS_SIZE; j++){
      curVal = this->map[i*RADIUS_SIZE + j];
      setImagePixel(w, curVal);
    }
  }
  
  w->output_image();
  delete w;
}


void HoughGrid::setImagePixel(PPMwriter* w, unsigned short val){
  //open map square
  if (val > 0) {
    float frac = 2048.0 / (float)val;
    frac = 255.0 - frac;
    if (frac < 0.0) frac = 0.0;
    w->write_pixel(0x00, (char)frac, 0xff);
  }
  else w->write_pixel(0x80, 0x80, 0x80);    //unknown cell
}

