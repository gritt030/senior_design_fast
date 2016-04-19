
#include "houghpeak.h"

//constructor, create grid
HoughPeak::HoughPeak(float peak1) {
  this->peak1map = new unsigned short[RADIUS_SIZE * THETA_BOUND]();
  this->peak2map = new unsigned short[RADIUS_SIZE * THETA_BOUND]();
  
  this->D_THETA = PEAK_DEGREES * 0.0174532925 / THETA_SIZE;
  
  SIN_ARRAY = new float[THETA_BOUND];
  COS_ARRAY = new float[THETA_BOUND];
  
  float theta = peak1 - PEAK_DEGREES*0.0174532925f;
  for (int i=0; i<THETA_BOUND; i++){
    SIN_ARRAY[i] = (float)sin(theta);
    COS_ARRAY[i] = (float)cos(theta);
    theta += D_THETA;
  }
}


//destructor, destroy grid
HoughPeak::~HoughPeak() {
  delete[] this->peak1map;
  delete[] this->peak2map;
  delete[] this->SIN_ARRAY;
  delete[] this->COS_ARRAY;
}


//add the hough transform of a point into the hough grid
void HoughPeak::addHoughPoint(int x, int y){
  this->Num_Points++;
  int dx = x-CENTER;
  int dy = y-CENTER;
  int rad;
  
  for (int i=0; i<THETA_BOUND; i++) {
    //add to peak 1 map
    rad = (int)(dx*COS_ARRAY[i] + dy*SIN_ARRAY[i]);
    rad += ADDITION;
    this->peak1map[i*RADIUS_SIZE + rad]++;
    
    //add to peak 2 map
    rad = (int)(dy*COS_ARRAY[i] - dx*SIN_ARRAY[i]);
    rad += ADDITION;
    this->peak2map[i*RADIUS_SIZE + rad]++;
  }
}


//get the highest peak for each theta column
int HoughPeak::getThetaPeaks(int* sumpeak1, int* sumpeak2){
  unsigned short curMax1, curMax2;
  unsigned short curVal1, curVal2;
    
  for (int i=0; i<THETA_BOUND; i++){
    curMax1 = 0;
    curMax2 = 0;
    
    for (int j=0; j<RADIUS_SIZE; j++){
      curVal1 = this->peak1map[i*RADIUS_SIZE + j];
      curVal2 = this->peak2map[i*RADIUS_SIZE + j];
      if (curVal1 > curMax1) curMax1 = curVal1;
      if (curVal2 > curMax2) curMax2 = curVal2;
    }
    
    sumpeak1[i] = (int)curMax1;
    sumpeak2[i] = (int)curMax2;
  }
  
  return this->Num_Points;
}

