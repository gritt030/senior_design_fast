#include "houghtransform.h"



HoughTransform::HoughTransform(OccupancyGrid* grid){
  this->grid = grid;
  this->houghGrid = new HoughGrid();
}


HoughTransform::HoughTransform(int numPoints, std::vector<int>* xpos, std::vector<int>* ypos){
  this->houghGrid = new HoughGrid();
  this->numPoints = numPoints;
  this->xPos = *xpos;
  this->yPos = *ypos;
}


HoughTransform::~HoughTransform(){
  delete this->houghGrid;
  delete this->houghPeak;
}


float HoughTransform::getYCardinal(){
  if (!isDone) {
    detectCardinalDirections();
    isDone = true;
  }
  
  return Y_Cardinal;
}
  
float HoughTransform::getXCardinal(){
  if (!isDone) {
    detectCardinalDirections();
    isDone = true;
  }
  
  return X_Cardinal;
}


void HoughTransform::performHoughTransform(){

  for (int i=0; i<numPoints; i++){
    this->houghGrid->addHoughPoint(xPos[i], yPos[i]);
  }
}



void HoughTransform::performHoughPeaks(){

  for (int i=0; i<numPoints; i++){
    this->houghPeak->addHoughPoint(xPos[i], yPos[i]);
  }
}



void HoughTransform::detectCardinalDirections(){
  ///// perform rough hough transform on given grid /////
  this->performHoughTransform();
  //this->houghGrid->sendHoughToImage("/home/owner/pics/pics/hough.ppm");
  
  int size = HoughGrid::THETA_SIZE;
  
  //generate sums for each theta value from 0 to 180
  int* hist = new int[size]();
  this->houghGrid->getThetaPeaks(hist);
  
  //find maximum 90 degree peaks in histogram
  int int90 = size >> 1;
  int mindex = 0;
  int mval = hist[0] + hist[int90];
  
  for (int i=1; i<int90; i++){
    if ((hist[i] + hist[i+int90]) > mval) {
      mindex = i;
      mval = (hist[i] + hist[i+int90]);
    }
  }
  
  delete[] hist;
  
  ///// perform a more accurate hough transform around the peaks /////
  float peak1 = 3.141592654f * ((float)mindex / (float)size);
    
  this->houghPeak = new HoughPeak(peak1);
  
  this->performHoughPeaks();
  size = HoughPeak::THETA_BOUND;
  
  int* hist1 = new int[size]();
  int* hist2 = new int[size]();
  
  this->houghPeak->getThetaPeaks(hist1, hist2);
    
  ///// calculate sums and weights for least squares /////
  int histWeight1 = 0;
  int histWeight2 = 0;
  int thetaWeight1 = 0;
  int thetaWeight2 = 0;
  
  for (int i=0; i<size; i++) {
    histWeight1 += hist1[i];
    histWeight2 += hist2[i];
    thetaWeight1 += i*hist1[i];
    thetaWeight2 += i*hist2[i];
  }
  
  delete[] hist1;
  delete[] hist2;
  
  //make sure that there were actually peaks in the hough transform
  if ((histWeight1 + histWeight2) == 0) {
    X_Cardinal = 1.570796327f;
    Y_Cardinal = 0.0;
    return;
  }
  
  //least squares for finding best cardinal direction fit
  float theta = (float)(thetaWeight1 + thetaWeight2) / (float)(histWeight1 + histWeight2);
  theta -= HoughPeak::THETA_SIZE;
  theta *= HoughPeak::PEAK_DEGREES * 0.0174532925f;
  theta /= HoughPeak::THETA_SIZE;
  theta += peak1;
    
  //save cardinal directions
  if ((theta > 0.7853981634f) && (theta < 2.35619449f)) {
    X_Cardinal = theta;
    Y_Cardinal = theta + 1.570796327f;
    if (Y_Cardinal > 3.141592654f) Y_Cardinal -= 3.141592654f;
  } else {
    Y_Cardinal = theta;
    X_Cardinal = theta + 1.570796327f;
    if (X_Cardinal > 3.141592654f) X_Cardinal -= 3.141592654f;
  }
}

