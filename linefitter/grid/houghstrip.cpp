#include "houghstrip.h"

//constructor, create grid
HoughStrip::HoughStrip() {
  this->D_THETA = 3.141592654 / THETA_SIZE;
  
  SIN_ARRAY = new float[THETA_SIZE];
  COS_ARRAY = new float[THETA_SIZE];
  TAN_ARRAY = new float[THETA_SIZE];
  
  X1_MIN_ARRAY = new int[THETA_SIZE]();
  Y1_MIN_ARRAY = new int[THETA_SIZE]();
  X1_MAX_ARRAY = new int[THETA_SIZE]();
  Y1_MAX_ARRAY = new int[THETA_SIZE]();
  COUNT_MIN_ARRAY = new int[THETA_SIZE]();
  MIN1_VALUE_ARRAY = new int[THETA_SIZE]();
  MIN2_VALUE_ARRAY = new int[THETA_SIZE]();
  X2_MIN_ARRAY = new int[THETA_SIZE]();
  Y2_MIN_ARRAY = new int[THETA_SIZE]();
  X2_MAX_ARRAY = new int[THETA_SIZE]();
  Y2_MAX_ARRAY = new int[THETA_SIZE]();
  COUNT_MAX_ARRAY = new int[THETA_SIZE]();
  MAX1_VALUE_ARRAY = new int[THETA_SIZE]();
  MAX2_VALUE_ARRAY = new int[THETA_SIZE]();
  
  float theta = 0.0;
  for (int i=0; i<THETA_SIZE; i++){
    SIN_ARRAY[i] = (float)sin(theta);
    COS_ARRAY[i] = (float)cos(theta);
    TAN_ARRAY[i] = (float)tan(theta);
    if (tan(theta) > 1000.0f) TAN_ARRAY[i] = 1000.0f;
    if (tan(theta) < -1000.0f) TAN_ARRAY[i] = -1000.0f;
    
    theta += D_THETA;
  }
  
  for (int i=0; i<THETA_SIZE; i++){
    MAX2_VALUE_ARRAY[i] = 10000;
    MIN1_VALUE_ARRAY[i] = -10000;
  }
}

//destructor, destroy grid
HoughStrip::~HoughStrip() {
  delete[] this->SIN_ARRAY;
  delete[] this->COS_ARRAY;
  delete[] this->TAN_ARRAY;
  delete[] this->X1_MIN_ARRAY;
  delete[] this->Y1_MIN_ARRAY;
  delete[] this->X1_MAX_ARRAY;
  delete[] this->Y1_MAX_ARRAY;
  delete[] this->COUNT_MIN_ARRAY;
  delete[] this->X2_MIN_ARRAY;
  delete[] this->Y2_MIN_ARRAY;
  delete[] this->X2_MAX_ARRAY;
  delete[] this->Y2_MAX_ARRAY;
  delete[] this->COUNT_MAX_ARRAY;
  delete[] this->MIN1_VALUE_ARRAY;
  delete[] this->MAX1_VALUE_ARRAY;
  delete[] this->MIN2_VALUE_ARRAY;
  delete[] this->MAX2_VALUE_ARRAY;
}


//add the hough transform of a point into the hough grid
void HoughStrip::addHoughPoint(int x, int y){
  this->Num_Points++;
  int dx = x-CENTER;
  int dy = y-CENTER;
  int rad;
  
  //get tangent of zero-slope theta
  float tanphi0;
  if (dx != 0.0) tanphi0 = (float)dy / (float)dx;
  else if (dy < 0) tanphi0 = -1000.0f;
  else tanphi0 = 1000.0f;
  
  //match zero-slope tangent to actual tangent
  int mindex = getTanIndex(tanphi0);
  
  //calculate zero-slope value
  rad = (int)(dx*COS_ARRAY[mindex] + dy*SIN_ARRAY[mindex]);
  
  //add point to strips
  //negative radiuses
  if (rad < 0) {
    COUNT_MIN_ARRAY[mindex]++;
    
    if (rad > MIN1_VALUE_ARRAY[mindex]){
      MIN1_VALUE_ARRAY[mindex] = rad;
      X1_MIN_ARRAY[mindex] = dx;
      Y1_MIN_ARRAY[mindex] = dy;
    } else if (rad < MIN2_VALUE_ARRAY[mindex]){
      MIN2_VALUE_ARRAY[mindex] = rad;
      X2_MIN_ARRAY[mindex] = dx;
      Y2_MIN_ARRAY[mindex] = dy;
    }
  
  //positive radiuses
  } else {
    COUNT_MAX_ARRAY[mindex]++;
    
    if (rad > MAX1_VALUE_ARRAY[mindex]){
      MAX1_VALUE_ARRAY[mindex] = rad;
      X1_MAX_ARRAY[mindex] = dx;
      Y1_MAX_ARRAY[mindex] = dy;
    } else if (rad < MAX2_VALUE_ARRAY[mindex]){
      MAX2_VALUE_ARRAY[mindex] = rad;
      X2_MAX_ARRAY[mindex] = dx;
      Y2_MAX_ARRAY[mindex] = dy;
    }
  }
}


//get the highest peak for each theta column
int HoughStrip::getThetaPeaks(int* sums){
  int dx, dy;
  float tanphi;
  int mindex;
  
  for (int i=0; i<THETA_SIZE; i++){
    std::cout << MAX1_VALUE_ARRAY[i] << " ";
    std::cout << MAX2_VALUE_ARRAY[i] << " ";
    std::cout << MIN1_VALUE_ARRAY[i] << " ";
    std::cout << MIN2_VALUE_ARRAY[i] << " ";
    std::cout << COUNT_MAX_ARRAY[i] << " ";
    std::cout << COUNT_MIN_ARRAY[i] << " ";
    std::cout << std::endl;
  }
  
  for (int i=0; i<THETA_SIZE; i++){
    if ((X1_MAX_ARRAY[i] != 0) || (X2_MAX_ARRAY[i] != 0) || (Y1_MAX_ARRAY[i] != 0) || (Y2_MAX_ARRAY[i] != 0)){
      dx = X1_MAX_ARRAY[i] - X2_MAX_ARRAY[i];
      dy = Y2_MAX_ARRAY[i] - Y1_MAX_ARRAY[i];

      if (dy != 0.0) tanphi = (float)dx / (float)dy;
      else if (dx < 0) tanphi = -1000.0f;
      else tanphi = 1000.0f;
      mindex = getTanIndex(tanphi);
      sums[mindex] += COUNT_MAX_ARRAY[i];
    }
    
    
    if ((X1_MIN_ARRAY[i] != 0) || (X2_MIN_ARRAY[i] != 0) || (Y1_MIN_ARRAY[i] != 0) || (Y2_MIN_ARRAY[i] != 0)){
      dx = X1_MIN_ARRAY[i] - X2_MIN_ARRAY[i];
      dy = Y2_MIN_ARRAY[i] - Y1_MIN_ARRAY[i];

      if (dy != 0.0) tanphi = (float)dx / (float)dy;
      else if (dx < 0) tanphi = -1000.0f;
      else tanphi = 1000.0f;
      mindex = getTanIndex(tanphi);
      sums[mindex] += COUNT_MIN_ARRAY[i];
    }
  }
  
  for (int i=0; i<THETA_SIZE; i++){
    //std::cout << i << ", " << sums[i] << std::endl;
  }
  
  return this->Num_Points;
}


int HoughStrip::getTanIndex(float tan){
  int mindex = 0;
  float minDiff = fabs(tan - TAN_ARRAY[0]);
  
  float curDiff;
  for (int i=1; i<THETA_SIZE; i++){
    curDiff = fabs(tan - TAN_ARRAY[i]);
    if (curDiff < minDiff) {
      mindex = i;
      minDiff = curDiff;
    }
  }
  
  return mindex;
}

