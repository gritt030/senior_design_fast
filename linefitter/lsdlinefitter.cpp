#include "lsdlinefitter.h"

#ifndef M_LN10
#define M_LN10 2.30258509299404568402f
#endif

#ifndef M_PI
#define M_PI   3.14159265358979323846f
#endif

#ifndef M_HPI
#define M_HPI  1.57079632679489661923f
#endif

#ifndef DIST
#define DIST(x1, y1, x2, y2) ((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1))
#endif

LsdLineFitter::LsdLineFitter() {
  int at = (int)round(ANG_TH * 1.41666666666667f);
  if (at > 254) ALIGNED_THRESH_CHAR = 254;
  else ALIGNED_THRESH_CHAR = (unsigned char)at;
  
  ALIGNED_THRESH_FLOAT = ANG_TH*M_HPI/180.0f;
}

LsdLineFitter::~LsdLineFitter()
{
  if (lsdimage != nullptr) delete lsdimage;
}


void LsdLineFitter::deleteRegion(Region* reg){
  delete[] reg->xVals;
  delete[] reg->yVals;
  delete reg;
}


//set the input image used to find lines
void LsdLineFitter::setImage(Grid* input){
  this->inimage = input;
}


//use the input image to generate the LSD image
void LsdLineFitter::generateLsdImage(){
  if (this->inimage == nullptr) return;
  
  int size = Grid::GRID_SIZE;
  if (SCALE != 1.0f) size = (int)(size*SCALE) + 1;
  
  this->lsdimage = new LsdGrid(size);
  this->LSD_GRID_SIZE = size;
  this->MAX_X = 0;
  this->MAX_Y = 0;
  this->MIN_X = size;
  this->MIN_Y = size;
  
  //copy input image without scaling it
  if (SCALE == 1.0) {
    char cur;
    for (int i=0; i<Grid::GRID_SIZE; i++){
      for (int j=0; j<Grid::GRID_SIZE; j++){
        cur = this->inimage->map[i*Grid::GRID_SIZE + j];
        if (cur < 0) {
          this->lsdimage->map[i*LSD_GRID_SIZE + j] = DEFINED;
          MIN_X = std::min(MIN_X, j);
          MIN_Y = std::min(MIN_Y, i);
          MAX_X = std::max(MAX_X, j);
          MAX_Y = std::max(MAX_Y, i);
        } else
          this->lsdimage->map[i*LSD_GRID_SIZE + j] = UNDEFINED;
      }
    }
  
  //scale image while copying it
  } else {
    int x, y;
    char cur;
    
    for (int i=0; i<Grid::GRID_SIZE; i++){
      for (int j=0; j<Grid::GRID_SIZE; j++){
        cur = this->inimage->map[i*Grid::GRID_SIZE + j];
        x = (int)(j*SCALE);
        y = (int)(i*SCALE);
        if (cur < 0) {
          this->lsdimage->map[y*LSD_GRID_SIZE + x] = DEFINED;
          MIN_X = std::min(MIN_X, x);
          MIN_Y = std::min(MIN_Y, y);
          MAX_X = std::max(MAX_X, x);
          MAX_Y = std::max(MAX_Y, y);
        } else {
          if (this->lsdimage->map[y*LSD_GRID_SIZE + x] != DEFINED)
            this->lsdimage->map[y*LSD_GRID_SIZE + x] = UNDEFINED;
        }
      }
    }
  }
  
  MAX_X++;
  MAX_Y++;
  
  MIN_REG_SIZE = (int)(-(5.0f*log10(size) + 1.041392685f) / log10(ANG_TH/180.0f));
//   MIN_REG_SIZE = 0;
  //std::cout << MIN_REG_SIZE << std::endl;
  
//   this->sendLsdToImage("/home/owner/pics/pics/lsdmake.ppm");
  this->blurImageX();
  this->blurImageY();
  crosshatchLsdImage();
//   this->sendLsdToImage("/home/owner/pics/pics/lsdblur.ppm");
}


void LsdLineFitter::crosshatchLsdImage(){
  int size = 65;
  for (int i=0; i<LSD_GRID_SIZE; i++){
    for (int j=(i%size); j<LSD_GRID_SIZE; j+=size){
      lsdimage->setValue(j,i, UNDEFINED);
    }
  }
  for (int i=1; i<LSD_GRID_SIZE; i++){
    for (int j=((i-1)%size); j<LSD_GRID_SIZE; j+=size){
      lsdimage->setValue(j,i, UNDEFINED);
    }
  }
  
  for (int i=LSD_GRID_SIZE-1; i>=0; i--){
    for (int j=LSD_GRID_SIZE-(i%size); j>=0; j-=size){
      lsdimage->setValue(j,i, UNDEFINED);
    }
  }
  for (int i=LSD_GRID_SIZE-2; i>=0; i--){
    for (int j=LSD_GRID_SIZE-((i-1)%size); j>=0; j-=size){
      lsdimage->setValue(j,i, UNDEFINED);
    }
  }
//   this->sendLsdToImage("/home/owner/pics/pics/lsdcross.ppm");
}


void LsdLineFitter::detectLineSegments(OccupancyGrid* grid, OccupancyGrid* newGrid){
  this->inimage = grid->grid;
  
  std::chrono::high_resolution_clock::time_point t1_1 = std::chrono::high_resolution_clock::now();
  this->generateLsdImage();
  //this->sendLsdToImage("/home/owner/pics/pics/lsd.ppm");
  std::chrono::high_resolution_clock::time_point t1_2 = std::chrono::high_resolution_clock::now();
  
  unsigned char curVal;
  Region* curRegion;
  Rect* curRect;
  
  float curLen;
  float dx, cx, dy, cy;
  float slope, x, y;
  
  long long access = 0;
  long long grow = 0;
  long long rect = 0;
  long long refine = 0;
  long long scale = 0;
  long long dist = 0;
  long long draw = 0;
  
  std::chrono::high_resolution_clock::time_point tL_1, tL_2;
  
  std::chrono::high_resolution_clock::time_point t2_1 = std::chrono::high_resolution_clock::now();
  for (int i=MIN_Y; i<MAX_Y; i++){
    for (int j=MIN_X; j<MAX_X; j++){
      tL_1 = std::chrono::high_resolution_clock::now();
      curVal = this->lsdimage->map[i*LSD_GRID_SIZE + j];
      tL_2 = std::chrono::high_resolution_clock::now();
      access += std::chrono::duration_cast<std::chrono::nanoseconds>(tL_2-tL_1).count();
      
      if (curVal != UNDEFINED){
      tL_1 = std::chrono::high_resolution_clock::now();
        curRegion = regionGrow(j,i);
      tL_2 = std::chrono::high_resolution_clock::now();
      grow += std::chrono::duration_cast<std::chrono::nanoseconds>(tL_2-tL_1).count();
        
        //reject small regions
        if (curRegion->size < MIN_REG_SIZE) continue;
        
      tL_1 = std::chrono::high_resolution_clock::now();
        curRect = regionToRect(curRegion);
      tL_2 = std::chrono::high_resolution_clock::now();
      rect += std::chrono::duration_cast<std::chrono::nanoseconds>(tL_2-tL_1).count();
        
      tL_1 = std::chrono::high_resolution_clock::now();
        refineRect(curRect, curRegion);
      tL_2 = std::chrono::high_resolution_clock::now();
      refine += std::chrono::duration_cast<std::chrono::nanoseconds>(tL_2-tL_1).count();
        
      tL_1 = std::chrono::high_resolution_clock::now();
        if (SCALE != 1.0f) {
          (curRect->x) /= SCALE;
          (curRect->y) /= SCALE;
          (curRect->x1) /= SCALE;
          (curRect->x2) /= SCALE;
          (curRect->y1) /= SCALE;
          (curRect->y2) /= SCALE;
          (curRect->width) /= SCALE;
          (curRect->dx) /= SCALE;
          (curRect->dy) /= SCALE;
        }
      tL_2 = std::chrono::high_resolution_clock::now();
      scale += std::chrono::duration_cast<std::chrono::nanoseconds>(tL_2-tL_1).count();
        
        
        
      tL_1 = std::chrono::high_resolution_clock::now();
        curLen = DIST(curRect->x1, curRect->y1, curRect->x2, curRect->y2);
      tL_2 = std::chrono::high_resolution_clock::now();
      dist += std::chrono::duration_cast<std::chrono::nanoseconds>(tL_2-tL_1).count();
        
        
      tL_1 = std::chrono::high_resolution_clock::now();
        if (curLen >= LENGTH){
          //horizontal lines
          if (isAligned(curRect->theta, 0.0f)){
            dx = curRect->x2 - curRect->x1;
            cx = dx/fabs(dx);
            dy = curRect->y2 - curRect->y1;
            cy = dy/fabs(dy);
            slope = dy/dx;
                      
            x = curRect->x1;
            y = curRect->y1;
            
            for (int l=0; l<=abs(dx); l++){
              newGrid->grid->setValue((int)x, (int)y, (char)-10);
              x += cx;
              y = (x - curRect->x1) * slope + curRect->y1;
            }
          
          //vertical lines
          } else if (isAligned(curRect->theta, 1.570796327f)){
            dx = curRect->x2 - curRect->x1;
            cx = dx/fabs(dx);
            dy = curRect->y2 - curRect->y1;
            cy = dy/fabs(dy);
            slope = dx/dy;
            
            x = curRect->x1;
            y = curRect->y1;
            
            for (int l=0; l<=abs(dy); l++){
              newGrid->grid->setValue((int)x, (int)y, (char)-10);
              y += cy;
              x = (y - curRect->y1) * slope + curRect->x1;
            }
            
          //region is not aligned to cardinal direction
          } else {
            for (int k=0; k<curRegion->size; k++){
              lsdimage->setValue(curRegion->xVals[k],curRegion->yVals[k], DEFINED);
            }
          }
        }
      tL_2 = std::chrono::high_resolution_clock::now();
      draw += std::chrono::duration_cast<std::chrono::nanoseconds>(tL_2-tL_1).count();
        
        
        delete curRect;
        deleteRegion(curRegion);
      }
    }
  }
  std::chrono::high_resolution_clock::time_point t2_2 = std::chrono::high_resolution_clock::now();
  
  
  //genimg loop access grow rect refine scale dist draw
  long long nano = std::chrono::duration_cast<std::chrono::nanoseconds>(t1_2-t1_1).count();
  std::cout << nano << " ";
  nano = std::chrono::duration_cast<std::chrono::nanoseconds>(t2_2-t2_1).count();
  std::cout << nano << " ";
  std::cout << access << " ";
  std::cout << grow << " ";
  std::cout << rect << " ";
  std::cout << refine << " ";
  std::cout << scale << " ";
  std::cout << dist << " ";
  std::cout << draw << std::endl;
}


void LsdLineFitter::detectLineSegmentsX(OccupancyGrid* grid, OccupancyGrid* newGrid){
  this->inimage = grid->grid;
  
  this->generateLsdImage();
  
  unsigned char curVal;
  Region* curRegion;
  Rect* curRect;
  
  float curLen;
  float dx, cx, dy, cy;
  float slope, x, y;
    
  for (int i=MIN_Y; i<MAX_Y; i++){
    for (int j=MIN_X; j<MAX_X; j++){
      curVal = this->lsdimage->map[i*LSD_GRID_SIZE + j];
      
      if (curVal != UNDEFINED){
        curRegion = regionGrow(j,i);
        curRect = regionToRect(curRegion);
        refineRect(curRect, curRegion);
        
        if (SCALE != 1.0f) {
          (curRect->x) /= SCALE;
          (curRect->y) /= SCALE;
          (curRect->x1) /= SCALE;
          (curRect->x2) /= SCALE;
          (curRect->y1) /= SCALE;
          (curRect->y2) /= SCALE;
          (curRect->width) /= SCALE;
          (curRect->dx) /= SCALE;
          (curRect->dy) /= SCALE;
        }
        
        curLen = DIST(curRect->x1, curRect->y1, curRect->x2, curRect->y2);
        
        if (curLen >= LENGTH){
          //horizontal lines
          if (isAligned(curRect->theta, 0.0f)){
            dx = curRect->x2 - curRect->x1;
            cx = dx/fabs(dx);
            dy = curRect->y2 - curRect->y1;
            cy = dy/fabs(dy);
            slope = dy/dx;
                      
            x = curRect->x1;
            y = curRect->y1;
            
            for (int l=0; l<=abs(dx); l++){
              newGrid->grid->setValue((int)x, (int)y, (char)-10);
              x += cx;
              y = (x - curRect->x1) * slope + curRect->y1;
            }
            
          //region is not aligned to cardinal direction
          } else {
            for (int k=0; k<curRegion->size; k++){
              lsdimage->setValue(curRegion->xVals[k],curRegion->yVals[k], DEFINED);
            }
          }
        }
          
        delete curRect;
        deleteRegion(curRegion);
      }
    }
  }
}


void LsdLineFitter::detectLineSegmentsY(OccupancyGrid* grid, OccupancyGrid* newGrid){
  this->inimage = grid->grid;
  
  this->generateLsdImage();
  this->sendLsdToImage("/home/owner/pics/pics/lsd.ppm");
  
  unsigned char curVal;
  Region* curRegion;
  Rect* curRect;
  
  float curLen;
  float dx, cx, dy, cy;
  float slope, x, y;
    
  for (int i=MIN_Y; i<MAX_Y; i++){
    for (int j=MIN_X; j<MAX_X; j++){
      curVal = this->lsdimage->map[i*LSD_GRID_SIZE + j];
      
      if (curVal != UNDEFINED){
        curRegion = regionGrow(j,i);
        curRect = regionToRect(curRegion);
        refineRect(curRect, curRegion);
        
        if (SCALE != 1.0f) {
          (curRect->x) /= SCALE;
          (curRect->y) /= SCALE;
          (curRect->x1) /= SCALE;
          (curRect->x2) /= SCALE;
          (curRect->y1) /= SCALE;
          (curRect->y2) /= SCALE;
          (curRect->width) /= SCALE;
          (curRect->dx) /= SCALE;
          (curRect->dy) /= SCALE;
        }
        
        curLen = DIST(curRect->x1, curRect->y1, curRect->x2, curRect->y2);
        
        if (curLen >= LENGTH){
          //vertical lines
          if (isAligned(curRect->theta, 1.570796327f)){
            dx = curRect->x2 - curRect->x1;
            cx = dx/fabs(dx);
            dy = curRect->y2 - curRect->y1;
            cy = dy/fabs(dy);
            slope = dx/dy;
            
            x = curRect->x1;
            y = curRect->y1;
            
            for (int l=0; l<=abs(dy); l++){
              newGrid->grid->setValue((int)x, (int)y, (char)-10);
              y += cy;
              x = (y - curRect->y1) * slope + curRect->x1;
            }
            
          //region is not aligned to cardinal direction
          } else {
            for (int k=0; k<curRegion->size; k++){
              lsdimage->setValue(curRegion->xVals[k],curRegion->yVals[k], DEFINED);
            }
          }
        }
          
        delete curRect;
        deleteRegion(curRegion);
      }
    }
  }
}




//grow a rectangular region
LsdLineFitter::Region* LsdLineFitter::regionGrow(int x, int y){
  int xx,yy,i, dx,dy;
  
  int regSize;
  PointList* ptList = new PointList;
  PointList* curPt = ptList;
  PointList* iterPt = ptList;

  //first point of the region
  regSize = 1;
  ptList->x = x;
  ptList->y = y;
  lsdimage->setValue(x,y,UNDEFINED);

  //try neighbors of existing region points as new points in region
  for(i=0; i<regSize; i++) {
    for (xx=(iterPt->x-1); xx<=(iterPt->x+1); xx++){
      for (yy=(iterPt->y-1); yy<=(iterPt->y+1); yy++){
        
        //current pixel is edge and part of region
        if (lsdimage->map[yy*LSD_GRID_SIZE + xx] != UNDEFINED) {
          //add point to pointlist
          PointList* pt = new PointList;
          pt->x = xx;
          pt->y = yy;
          //lsdimage->setValue(xx,yy,UNDEFINED);
          lsdimage->map[yy*LSD_GRID_SIZE + xx] = UNDEFINED;
          curPt->next = pt;
          curPt = pt;
          
          //increment size of region
          regSize++;
        }
      }
    }
    
    //move to next point in pointlist
    iterPt = iterPt->next;
  }
  
  //create region to return
  Region* reg = new Region;
  reg->size = regSize;
  reg->xVals = new int[regSize];
  reg->yVals = new int[regSize];
  
  iterPt = ptList;
  
  for (i=0; i<regSize; i++){
    //add point to region
    reg->xVals[i] = iterPt->x;
    reg->yVals[i] = iterPt->y;
    
    //delete point in linked list
    ptList = iterPt->next;
    delete iterPt;
    iterPt = ptList;
  }
  
  //return new region
  return reg;
}


bool LsdLineFitter::isAligned(float angle1, float angle2){
  float diff = fabs(angle1 - angle2);
  
  if (diff <= ALIGNED_THRESH_FLOAT) return true;
  else if (diff >= (M_PI - ALIGNED_THRESH_FLOAT)) return true;
  else return false;
}


float LsdLineFitter::getTheta(Region* reg, float x, float y){
  float lambda,theta;
  float Ixx = 0.0f;
  float Iyy = 0.0f;
  float Ixy = 0.0f;
  int i;

  //compute inertia matrix
  for(i=0; i<reg->size; i++)
  {
    Ixx += ((float)reg->yVals[i] - y) * ((float)reg->yVals[i] - y);
    Iyy += ((float)reg->xVals[i] - x) * ((float)reg->xVals[i] - x);
    Ixy -= ((float)reg->xVals[i] - x) * ((float)reg->yVals[i] - y);
  }
  
  //compute smallest eigenvalue
  lambda = 0.5f * ( Ixx + Iyy - sqrt( (Ixx-Iyy)*(Ixx-Iyy) + 4.0f*Ixy*Ixy ) );
  
  //compute angle
  theta = fabs(Ixx)>fabs(Iyy) ? atan2(lambda-Ixx,Ixy) : atan2(Ixy,lambda-Iyy);
  
  //make positive
  if (theta < 0.0f) theta += M_PI;
  
  return theta;
}


float LsdLineFitter::getTheta2(Region* reg, float x, float y){
  float theta;
  float Ixx = 0.0f;
  float Iyy = 0.0f;
  float Ixy = 0.0f;
  int i;

  //compute inertia matrix
  for(i=0; i<reg->size; i++)
  {
    Ixx += ((float)reg->yVals[i] - y) * ((float)reg->yVals[i] - y);
    Iyy += ((float)reg->xVals[i] - x) * ((float)reg->xVals[i] - x);
    Ixy += ((float)reg->xVals[i] - x) * ((float)reg->yVals[i] - y);
  }
  
  Ixx /= reg->size;
  Iyy /= reg->size;
  Ixy /= reg->size;
  theta = 0.5 * atan2(-2*Ixy, -Iyy + Ixx);
  theta += M_HPI;
  
  return theta;
}


float LsdLineFitter::getTheta3(Region* reg, float x, float y){
  float theta;
  int Ixx = 0;
  int Iyy = 0;
  int Ixy = 0;
  int i;
  int cx = (int)round(x);
  int cy = (int)round(y);

  //compute inertia matrix
  for(i=0; i<reg->size; i++)
  {
    Ixx += (reg->yVals[i] - cy) * (reg->yVals[i] - cy);
    Iyy += (reg->xVals[i] - cx) * (reg->xVals[i] - cx);
    Ixy += (reg->xVals[i] - cx) * (reg->yVals[i] - cy);
  }
  
  float mxx = (float)Ixx / (float)reg->size;
  float myy = (float)Iyy / (float)reg->size;
  float mxy = (float)Ixy / (float)reg->size;
  theta = 0.5 * atan2(-2*mxy, -myy + mxx);
  theta += M_HPI;
  
  return theta;
}

//BEST ONE
float LsdLineFitter::getTheta4(Region* reg, float x, float y){
  float theta;
  int syy = 0;
  int sxx = 0;
  int sy = 0;
  int sx = 0;
  int sxy = 0;
  int i;

  //compute inertia matrix
  for(i=0; i<reg->size; i++)
  {
    syy += reg->yVals[i] * reg->yVals[i];
    sy += reg->yVals[i];
    sxy += reg->yVals[i] * reg->xVals[i];
    sxx += reg->xVals[i] * reg->xVals[i];
    sx += reg->xVals[i];
  }
  
  float Ixx = syy - 2.0f*y*sy + reg->size*y*y;
  float Iyy = sxx - 2.0f*x*sx + reg->size*x*x;
  float Ixy = sxy - x*sy - y*sx + reg->size*x*y;
  
  Ixx /= reg->size;
  Iyy /= reg->size;
  Ixy /= reg->size;
  theta = 0.5f * atan2(-2.0f*Ixy, -Iyy + Ixx);
  theta += M_HPI;
  
  return theta;
}


// LsdLineFitter::Rect* LsdLineFitter::regionToRect(Region* reg){
//   float x,y,dx,dy,l,w,theta,l_min,l_max,w_min,w_max;
//   int i;
//   
//   //compute needed sums
//   int sumX=0, sumY=0, sumXX=0, sumYY=0, sumXY=0;
//   for(i=0; i<reg->size; i++)
//   {
//     sumYY += reg->yVals[i] * reg->yVals[i];
//     sumY += reg->yVals[i];
//     sumXY += reg->yVals[i] * reg->xVals[i];
//     sumXX += reg->xVals[i] * reg->xVals[i];
//     sumX += reg->xVals[i];
//   }
// 
//   //center of region using LS
//   x = (float)sumX / (float)reg->size;
//   y = (float)sumY / (float)reg->size;
// 
//   //angle of region
// //   std::chrono::high_resolution_clock::time_point t1_1 = std::chrono::high_resolution_clock::now();
// //   for (int i=0; i<1000; i++){
// //     theta = getTheta(reg, x, y);
// //   }
// //   std::chrono::high_resolution_clock::time_point t1_2 = std::chrono::high_resolution_clock::now();
// //   
// //   std::chrono::high_resolution_clock::time_point t2_1 = std::chrono::high_resolution_clock::now();
// //   for (int i=0; i<1000; i++){
// //     theta = getTheta2(reg, x, y);
// //   }
// //   std::chrono::high_resolution_clock::time_point t2_2 = std::chrono::high_resolution_clock::now();
// //   
// //   std::chrono::high_resolution_clock::time_point t3_1 = std::chrono::high_resolution_clock::now();
// //   for (int i=0; i<1000; i++){
// //     theta = getTheta3(reg, x, y);
// //   }
// //   std::chrono::high_resolution_clock::time_point t3_2 = std::chrono::high_resolution_clock::now();
// //   
// //   std::chrono::high_resolution_clock::time_point t4_1 = std::chrono::high_resolution_clock::now();
// //   for (int i=0; i<1000; i++){
// //     theta = getTheta4(reg, x, y);
// //   }
// //   std::chrono::high_resolution_clock::time_point t4_2 = std::chrono::high_resolution_clock::now();
// //   
// //   long long nano = std::chrono::duration_cast<std::chrono::nanoseconds>(t1_2-t1_1).count();
// //   std::cout << nano << " ";
// //   nano = std::chrono::duration_cast<std::chrono::nanoseconds>(t2_2-t2_1).count();
// //   std::cout << nano << " ";
// //   nano = std::chrono::duration_cast<std::chrono::nanoseconds>(t3_2-t3_1).count();
// //   std::cout << nano << " ";
// //   nano = std::chrono::duration_cast<std::chrono::nanoseconds>(t4_2-t4_1).count();
// //   std::cout << nano << std::endl;
//   
// //   std::cout << getTheta(reg, x, y) << " ";
// //   std::cout << getTheta2(reg, x, y) << " ";
// //   std::cout << getTheta3(reg, x, y) << " ";
// //   std::cout << getTheta4(reg, x, y) << std::endl;
//   
// //   theta = getTheta4(reg, x, y);
//   
//   //find theta
//   float Ixx = sumYY - 2.0f*y*sumY + reg->size*y*y;
//   float Iyy = sumXX - 2.0f*x*sumX + reg->size*x*x;
//   float Ixy = sumXY - x*sumY - y*sumX + reg->size*x*y;
//   
//   Ixx /= reg->size;
//   Iyy /= reg->size;
//   Ixy /= reg->size;
//   theta = 0.5f * atan2(-2.0f*Ixy, -Iyy + Ixx);
//   theta += M_HPI;
// 
//   //length and width
//   dx = cos(theta);
//   dy = sin(theta);
//   l_min = l_max = w_min = w_max = 0.0f;
//   for(i=0; i<reg->size; i++)
//   {
//     l =  ((float)reg->xVals[i] - x) * dx + ((float)reg->yVals[i] - y) * dy;
//     w = -((float)reg->xVals[i] - x) * dy + ((float)reg->yVals[i] - y) * dx;
// 
//     if( l > l_max ) l_max = l;
//     if( l < l_min ) l_min = l;
//     if( w > w_max ) w_max = w;
//     if( w < w_min ) w_min = w;
//   }
//   
//   //store values
//   Rect* rec = new Rect;
//   rec->x1 = x + l_min * dx;
//   rec->y1 = y + l_min * dy;
//   rec->x2 = x + l_max * dx;
//   rec->y2 = y + l_max * dy;
//   rec->width = w_max - w_min;
//   rec->x = x;
//   rec->y = y;
//   rec->theta = theta;
//   rec->dx = dx;
//   rec->dy = dy;
// 
//   //we impose a minimal width of one pixel
//   if( rec->width < 1.0f ) rec->width = 1.0f;
//   
//   return rec;
// }




LsdLineFitter::Rect* LsdLineFitter::regionToRect(Region* reg){
  float x,y,dx,dy,l,w,theta,l_min,l_max,w_min,w_max;
  int i;
  
  //compute needed sums
  int sumX=0, sumY=0, sumXX=0, sumYY=0, sumXY=0;
  for(i=0; i<reg->size; i++)
  {
    sumYY += reg->yVals[i] * reg->yVals[i];
    sumY += reg->yVals[i];
    sumXY += reg->yVals[i] * reg->xVals[i];
    sumXX += reg->xVals[i] * reg->xVals[i];
    sumX += reg->xVals[i];
  }

  
  //center of region using LS
  x = (float)sumX / (float)reg->size;
  y = (float)sumY / (float)reg->size;

  
  //find angle of region
  float Ixx = sumYY - 2.0f*y*sumY + reg->size*y*y;
  float Iyy = sumXX - 2.0f*x*sumX + reg->size*x*x;
  float Ixy = sumXY - x*sumY - y*sumX + reg->size*x*y;
  
  Ixx /= reg->size;
  Iyy /= reg->size;
  Ixy /= reg->size;
  theta = 0.5f * atan2(-2.0f*Ixy, -Iyy + Ixx);
  theta += M_HPI;

  //length and width
  dx = cos(theta);
  dy = sin(theta);
  l_min = l_max = w_min = w_max = 0.0f;
  for(i=0; i<reg->size; i++)
  {
    l =  ((float)reg->xVals[i] - x) * dx + ((float)reg->yVals[i] - y) * dy;
    w = -((float)reg->xVals[i] - x) * dy + ((float)reg->yVals[i] - y) * dx;
    
    if( l > l_max ) l_max = l;
    if( l < l_min ) l_min = l;
    if( w > w_max ) w_max = w;
    if( w < w_min ) w_min = w;
  }
  
  //store values
  Rect* rec = new Rect;
  rec->x1 = x + l_min * dx;
  rec->y1 = y + l_min * dy;
  rec->x2 = x + l_max * dx;
  rec->y2 = y + l_max * dy;
  rec->width = w_max - w_min;
  rec->x = x;
  rec->y = y;
  rec->theta = theta;
  rec->dx = dx;
  rec->dy = dy;

  //we impose a minimal width of one pixel
  if( rec->width < 1.0f ) rec->width = 1.0f;
  
  return rec;
}



// LsdLineFitter::Rect* LsdLineFitter::regionToRect(Region* reg){
//   float x,y,theta,l_min,l_max;
//   int i;
//   
//   //compute needed sums
//   int sumX=0, sumY=0, sumXX=0, sumYY=0, sumXY=0;
//   for(i=0; i<reg->size; i++)
//   {
//     sumYY += reg->yVals[i] * reg->yVals[i];
//     sumY += reg->yVals[i];
//     sumXY += reg->yVals[i] * reg->xVals[i];
//     sumXX += reg->xVals[i] * reg->xVals[i];
//     sumX += reg->xVals[i];
//   }
// 
//   
//   //center of region using LS
//   x = (float)sumX / (float)reg->size;
//   y = (float)sumY / (float)reg->size;
// 
//   
//   //find angle of region
//   float Ixx = sumYY - 2.0f*y*sumY + reg->size*y*y;
//   float Iyy = sumXX - 2.0f*x*sumX + reg->size*x*x;
//   float Ixy = sumXY - x*sumY - y*sumX + reg->size*x*y;
//   
//   Ixx /= reg->size;
//   Iyy /= reg->size;
//   Ixy /= reg->size;
//   theta = 0.5f * atan2(-2.0f*Ixy, -Iyy + Ixx);
//   theta += M_HPI;
// 
//   
//   //length variables
//   int maxLX = reg->xVals[0];
//   int maxLY = reg->yVals[0];
//   int minLX = reg->xVals[0];
//   int minLY = reg->yVals[0];
//   
//   //width variables
//   int maxWY = reg->yVals[0];
//   int minWY = reg->yVals[0];
//   
//   //find max/min length/width
//   for(i=1; i<reg->size; i++)
//   {
//     if (reg->xVals[i] > maxLX){
//       maxLX = reg->xVals[i];
//       maxLY = reg->yVals[i];
//     } else if (reg->xVals[i] < minLX){
//       minLX = reg->xVals[i];
//       minLY = reg->yVals[i];
//     }
//     
//     if (reg->yVals[i] > maxWY){
//       maxWY = reg->yVals[i];
//     } else if (reg->yVals[i] < minWY){
//       minWY = reg->yVals[i];
//     }
//   }
//   
//   //transform to proper coordinate frame
//   float co = cos(theta);
//   float si = sin(theta);
//   l_min = (minLX - x)*co + (minLY - y)*si;
//   l_max = (maxLX - x)*co + (maxLY - y)*si;
//   
//   
//   //store values
//   Rect* rec = new Rect;
//   rec->x1 = x + l_min * co;
//   rec->y1 = y + l_min * si;
//   rec->x2 = x + l_max * co;
//   rec->y2 = y + l_max * si;
//   rec->width = maxWY - minWY;
//   rec->x = x;
//   rec->y = y;
//   rec->theta = theta;
//   rec->dx = co;
//   rec->dy = si;
//   
//   std::cout << rec->x1 << " ";
//   std::cout << rec->y1 << " ";
//   std::cout << rec->x2 << " ";
//   std::cout << rec->y2 << " ";
//   std::cout << rec->width << " ";
//   std::cout << rec->x << " ";
//   std::cout << rec->y << " ";
//   std::cout << rec->theta << " ";
//   std::cout << rec->dx << " ";
//   std::cout << rec->dy << std::endl;;
// 
//   //we impose a minimal width of one pixel
//   if( rec->width < 1.0f ) rec->width = 1.0f;
//   
//   return rec;
// }



bool LsdLineFitter::refineRect(Rect* rec, Region* reg){
  float density,xc,yc;
  float rad1,rad2,rad;
  int i;
  Rect* newRect;

  //compute region points density
  density = (float)(reg->size * reg->size) / (DIST(rec->x1,rec->y1,rec->x2,rec->y2) * rec->width * rec->width);

  //if the density criterion is satisfied there is nothing to do
  if( density >= DENSITY_TH ) return true;

  //-----------------Try reducing region radius----------------
  //compute region's radius
  xc = (float) reg->xVals[0];
  yc = (float) reg->yVals[0];
  rad1 = DIST( xc, yc, rec->x1, rec->y1 );
  rad2 = DIST( xc, yc, rec->x2, rec->y2 );
  rad = rad1 > rad2 ? rad1 : rad2;
  
  //while the density criterion is not satisfied, remove farther pixels
  while( density < DENSITY_TH ) {
    rad *= 0.5625f; //reduce region's radius to 75% of its value

    //remove points from the region and update 'used' map
    for(i=0; i<reg->size; i++)
      if(DIST(xc, yc, (float)reg->xVals[i], (float)reg->yVals[i]) > rad)
        {
          //point not kept, mark it as unused
          lsdimage->map[reg->yVals[i]*LSD_GRID_SIZE + reg->xVals[i]] = DEFINED;
          //lsdimage->setValue(reg->xVals[i],reg->yVals[i],DEFINED);
          
          //remove point from the region
          reg->xVals[i] = reg->xVals[reg->size-1];
          reg->yVals[i] = reg->yVals[reg->size-1];
          (reg->size)--;
          i--; //to avoid skipping one point
        }

    //reject if the region is too small.
    if( reg->size < 2 ) return false;

    //re-compute rectangle
    newRect = regionToRect(reg);
    rec->x1 = newRect->x1;
    rec->x2 = newRect->x2;
    rec->y1 = newRect->y1;
    rec->y2 = newRect->y2;
    rec->x = newRect->x;
    rec->y = newRect->y;
    rec->dx = newRect->dx;
    rec->dy = newRect->dy;
    rec->theta = newRect->theta;
    rec->width = newRect->width;
    delete newRect;

    //re-compute region points density
    density = (float)(reg->size * reg->size) / (DIST(rec->x1,rec->y1,rec->x2,rec->y2) * rec->width * rec->width);
  }

  //if this point is reached, the density criterion is satisfied
  return true;
}


void LsdLineFitter::blurImageX(){
  for (int i=MIN_Y; i<MAX_Y; i++){
    for (int j=MIN_X; j<MAX_X; j++){
      
      if (this->lsdimage->map[i*LSD_GRID_SIZE + j] != LsdGrid::UNDEFINED){
        for (int k=j-BLUR; k<=j+BLUR; k++){
          this->lsdimage->setValue(k, i, DEFINED);
        }
        j += BLUR;
      }
      
    }
  }
}


void LsdLineFitter::blurImageY(){
  for (int i=MIN_Y; i<MAX_Y; i++){
    for (int j=MIN_X; j<MAX_X; j++){
      
      if (this->lsdimage->map[j*LSD_GRID_SIZE + i] != LsdGrid::UNDEFINED){
        for (int k=j-BLUR; k<=j+BLUR; k++){
          this->lsdimage->setValue(i, k, DEFINED);
        }
        j += BLUR;
      }
      
    }
  }
}









void LsdLineFitter::sendLsdToImage(char* filename){
  PPMwriter* w = new PPMwriter();
  int size = this->lsdimage->GRID_SIZE;
  
  w->create_image(filename, size, size);
  
  for (int i=0; i<size; i++){
    for (int j=0; j<size; j++) {
      unsigned char cur = lsdimage->getValue(j, i);
      setLsdImagePixel(w, cur);
    }
  }
  
  w->output_image();
  delete w;
}


void LsdLineFitter::setLsdImagePixel(PPMwriter* w, unsigned char value){
  //open map square
  if (value != UNDEFINED) {
    w->write_pixel(0xff, 0x00, 0x00);
  
  //unknown map square
  } else {
    w->write_pixel(0x80, 0x80, 0x80);
  }
}

