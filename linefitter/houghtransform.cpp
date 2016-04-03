#include "houghtransform.h"



HoughTransform::HoughTransform(OccupancyGrid* grid){
  this->grid = grid;
  this->houghGrid = new HoughGrid();
}


HoughTransform::~HoughTransform(){
  delete this->houghGrid;
}


double HoughTransform::getYCardinal(){
  if (!isDone) {
    detectCardinalDirections();
    isDone = true;
  }
  
  return Y_Cardinal;
}
  
double HoughTransform::getXCardinal(){
  if (!isDone) {
    detectCardinalDirections();
    isDone = true;
  }
  
  return X_Cardinal;
}


void HoughTransform::performHoughTransform(){
  //generate hough transform grid
  for (int i=0; i<Grid::GRID_SIZE; i++){
    for (int j=0; j<Grid::GRID_SIZE; j++){
      char cur = this->grid->grid->map[j*Grid::GRID_SIZE + i];
      if (cur < 0) this->houghGrid->addHoughPoint(i,j);
    }
  }
}



void HoughTransform::detectCardinalDirections(){
  //perform hough transform on given grid
  this->performHoughTransform();
  
  //this->houghGrid->sendHoughToImage("/home/owner/pics/pics/hough.png");
  
  int size = HoughGrid::THETA_SIZE;
  int fit = (int)((180.0 / (double)HoughGrid::THETA_SIZE) * 10.0);
  
  //generate sums for each theta value from 0 to 180
  int* hist = new int[size]();
  this->houghGrid->getThetaSums(hist);
  
  //find minimum 90 degree peaks in histogram
  int int90 = size >> 1;
  int mindex = 0;
  int minval = hist[0] + hist[int90];
  
  for (int i=1; i<int90; i++){
    if ((hist[i] + hist[i+int90]) < minval) {
      mindex = i;
      minval = (hist[i] + hist[i+int90]);
    }
  }
    
  //calculate sums and weights for least squares
  int index;
  int index2;
  int histWeight1 = 0;
  int histWeight2 = 0;
  int thetaWeight1 = 0;
  int thetaWeight2 = 0;
  
  for (int i=(mindex-fit); i<=(mindex+fit); i++) {
    if (i < 0) {
      index = i+size;
      index2 = index - (size >> 1);
    } else {
      index = i % size;
      index2 = (i + (size >> 1)) % size;
    }
    
    histWeight1 += hist[index];
    histWeight2 += hist[index2];
    thetaWeight1 += i*hist[index];
    thetaWeight2 += i*hist[index2];
  }
  
  delete[] hist;
  
  //make sure that there were actually peaks in the hough transform
  if ((histWeight1 + histWeight2) == 0) {
    X_Cardinal = 1.570796327;
    Y_Cardinal = 0.0;
    return;
  }
  
  //least squares for finding best cardinal direction fit
  double theta = (double)(thetaWeight1 + thetaWeight2) / (double)(histWeight1 + histWeight2);
  theta = 3.141592654 * (theta / (double)size);
  
  //save cardinal directions
  if ((theta > 0.7853981634) && (theta < 2.35619449)) {
    X_Cardinal = theta;
    Y_Cardinal = theta + 1.570796327;
    if (Y_Cardinal > 3.141592654) Y_Cardinal -= 3.141592654;
  } else {
    Y_Cardinal = theta;
    X_Cardinal = theta + 1.570796327;
    if (X_Cardinal > 3.141592654) X_Cardinal -= 3.141592654;
  }
}

