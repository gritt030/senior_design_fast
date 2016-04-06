#include "lsdlinefitter.h"

#ifndef M_LN10
#define M_LN10 2.30258509299404568402
#endif

#ifndef M_PI
#define M_PI   3.14159265358979323846
#endif

#ifndef M_HPI
#define M_HPI  1.57079632679489661923
#endif

LsdLineFitter::LsdLineFitter() {
  int at = (int)round(ANG_TH * 1.41666666666667);
  if (at > 254) ALIGNED_THRESH_CHAR = 254;
  else ALIGNED_THRESH_CHAR = (unsigned char)at;
  
  ALIGNED_THRESH_DOUBLE = ANG_TH*M_HPI/180.0;
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
  if (SCALE != 1.0) size = (int)(size*SCALE) + 1;
  
  this->lsdimage = new LsdGrid(size);
  this->LSD_GRID_SIZE = size;
  
  //copy input image without scaling it
  if (SCALE == 1.0) {
    char cur;
    for (int i=0; i<Grid::GRID_SIZE; i++){
      for (int j=0; j<Grid::GRID_SIZE; j++){
        cur = this->inimage->map[j*Grid::GRID_SIZE + i];
        if (cur < 0)
          this->lsdimage->setValue(i,j,(unsigned char)1);
        else
          this->lsdimage->setValue(i,j,UNDEFINED);
      }
    }
  
  //scale image while copying it
  } else {
    int x, y;
    char cur;
    unsigned char curL;
    
    for (int i=0; i<Grid::GRID_SIZE; i++){
      for (int j=0; j<Grid::GRID_SIZE; j++){
        cur = this->inimage->map[j*Grid::GRID_SIZE + i];
        if (cur < 0) {
          this->lsdimage->setValue((int)(i*SCALE),(int)(j*SCALE),(unsigned char)1);
        } else {
          x = (int)(i*SCALE);
          y = (int)(j*SCALE);
          curL = this->lsdimage->getValue(i,j);
          if (curL != 1)
            this->lsdimage->setValue(x,y,UNDEFINED);
        }
      }
    }
  }
  
//   this->sendLsdToImage("/home/owner/pics/pics/lsd.ppm");
  this->blurImageX();
  this->blurImageY();
//   this->sendLsdToImage("/home/owner/pics/pics/lsdblur.ppm");
}


//set all wall pixels to have a gradient of 0 degrees
void LsdLineFitter::gradientImageX(){
  unsigned char cur;
  
  for (int i=0; i<LSD_GRID_SIZE; i++){
    for (int j=0; j<LSD_GRID_SIZE; j++){
      cur = this->lsdimage->map[j*LSD_GRID_SIZE + i];
      if (cur != UNDEFINED)
        lsdimage->setValue(i,j,(unsigned char)0);
    }
  }
}


//set all wall pixels to have a gradient of 90 degrees
void LsdLineFitter::gradientImageY(){
  unsigned char cur;
  
  for (int i=0; i<LSD_GRID_SIZE; i++){
    for (int j=0; j<LSD_GRID_SIZE; j++){
      cur = this->lsdimage->map[j*LSD_GRID_SIZE + i];
      if (cur != UNDEFINED)
        lsdimage->setValue(i,j,(unsigned char)128);
    }
  }
}


void LsdLineFitter::detectLineSegments(OccupancyGrid* grid, OccupancyGrid* newGrid){
  this->setImage(grid->grid);
  
  this->generateLsdImage();
  this->gradientImageX();
  
  unsigned char curVal;
  Region* curRegion;
  Rect* curRect;
  
  ///std::cout << lsdimage->GRID_SIZE << std::endl;
  
  for (int i=0; i<LSD_GRID_SIZE; i++){
    for (int j=0; j<LSD_GRID_SIZE; j++){
      curVal = this->lsdimage->map[j*LSD_GRID_SIZE + i];
      
      if (curVal != UNDEFINED){
        curRegion = regionGrow(i,j);
        curRect = regionToRect(curRegion);
        refineRect(curRect, curRegion);
        
        if (SCALE != 1.0) {
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
        
        double curLen = dist(curRect->x1, curRect->y1, curRect->x2, curRect->y2);
        
        if ((isAligned(curRect->theta, 0.0)) && (curLen >= LENGTH)){
          double dx = curRect->x2 - curRect->x1;
          double cx = dx/fabs(dx);
          double dy = curRect->y2 - curRect->y1;
          double cy = dy/fabs(dy);
          double slope = dy/dx;
                    
          double x = curRect->x1;
          double y = curRect->y1;
          
          for (double l=0.0; l<=fabs(dx); l+=1.0){
            newGrid->grid->setValue((int)x, (int)y, (char)-10);
            x += cx;
            y = (x - curRect->x1) * slope + curRect->y1;
          }
          
        } else {
          for (int k=0; k<curRegion->size; k++){
            lsdimage->setValue(curRegion->xVals[k],curRegion->yVals[k],curRegion->angle);
          }
        }
        
        delete curRect;
        deleteRegion(curRegion);
      }
    }
  }
  
  //this->sendLsdToImage("/home/owner/pics/pics/lsdx.ppm");
  
  
  //vertical lines
  this->gradientImageY();
    
  for (int i=0; i<LSD_GRID_SIZE; i++){
    for (int j=0; j<LSD_GRID_SIZE; j++){
      curVal = this->lsdimage->map[j*LSD_GRID_SIZE + i];
      
      if (curVal != UNDEFINED){
        curRegion = regionGrow(i,j);
        curRect = regionToRect(curRegion);
        refineRect(curRect, curRegion);
        
        if (SCALE != 1.0) {
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
        
        double curLen = dist(curRect->x1, curRect->y1, curRect->x2, curRect->y2);
        
        if ((isAligned(curRect->theta, 1.570796327)) && (curLen >= LENGTH)){
          double dx = curRect->x2 - curRect->x1;
          double cx = dx/fabs(dx);
          double dy = curRect->y2 - curRect->y1;
          double cy = dy/fabs(dy);
          double slope = dx/dy;
          
          double x = curRect->x1;
          double y = curRect->y1;
          
          for (double l=0.0; l<=fabs(dy); l+=1.0){
            newGrid->grid->setValue((int)x, (int)y, (char)-10);
            y += cy;
            x = (y - curRect->y1) * slope + curRect->x1;
          }
          
        } else {
          for (int k=0; k<curRegion->size; k++){
            lsdimage->setValue(curRegion->xVals[k],curRegion->yVals[k],curRegion->angle);
          }
        }
        
        delete curRect;
        deleteRegion(curRegion);
      }
    }
  }
  
  //this->sendLsdToImage("/home/owner/pics/pics/lsdy.ppm");
}


void LsdLineFitter::detectLineSegmentsX(OccupancyGrid* grid, OccupancyGrid* newGrid){
  this->setImage(grid->grid);
  
  this->generateLsdImage();
  this->gradientImageX();
  
  unsigned char curVal;
  Region* curRegion;
  Rect* curRect;
  
  std::cout << lsdimage->GRID_SIZE << std::endl;
  
  //horizontal lines
  for (int i=0; i<LSD_GRID_SIZE; i++){
    for (int j=0; j<LSD_GRID_SIZE; j++){
      curVal = this->lsdimage->map[j*LSD_GRID_SIZE + i];
      
      if (curVal != UNDEFINED){
        curRegion = regionGrow(i,j);
        curRect = regionToRect(curRegion);
        refineRect(curRect, curRegion);
        
        if (SCALE != 1.0) {
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
        
        double curLen = dist(curRect->x1, curRect->y1, curRect->x2, curRect->y2);
        
        if ((isAligned(curRect->theta, 0.0)) && (curLen >= LENGTH)){
          double dx = curRect->x2 - curRect->x1;
          double cx = dx/fabs(dx);
          double dy = curRect->y2 - curRect->y1;
          double cy = dy/fabs(dy);
          double slope = dy/dx;
                    
          double x = curRect->x1;
          double y = curRect->y1;
          
          for (double l=0.0; l<=fabs(dx); l+=1.0){
            newGrid->grid->setValue((int)x, (int)y, (char)-10);
            x += cx;
            y = (x - curRect->x1) * slope + curRect->y1;
          }
          
        } else {
          for (int k=0; k<curRegion->size; k++){
            lsdimage->setValue(curRegion->xVals[k],curRegion->yVals[k],curRegion->angle);
          }
        }
        
        delete curRect;
        deleteRegion(curRegion);
      }
    }
  }
}


void LsdLineFitter::detectLineSegmentsY(OccupancyGrid* grid, OccupancyGrid* newGrid){
  this->setImage(grid->grid);
  
  this->generateLsdImage();
  this->gradientImageY();
    
  unsigned char curVal;
  Region* curRegion;
  Rect* curRect;
    
  for (int i=0; i<LSD_GRID_SIZE; i++){
    for (int j=0; j<LSD_GRID_SIZE; j++){
      curVal = this->lsdimage->map[j*LSD_GRID_SIZE + i];
      
      if (curVal != UNDEFINED){
        curRegion = regionGrow(i,j);
        curRect = regionToRect(curRegion);
        refineRect(curRect, curRegion);
        
        if (SCALE != 1.0) {
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
        
        double curLen = dist(curRect->x1, curRect->y1, curRect->x2, curRect->y2);
        
        if ((isAligned(curRect->theta, 1.570796327)) && (curLen >= LENGTH)){
          double dx = curRect->x2 - curRect->x1;
          double cx = dx/fabs(dx);
          double dy = curRect->y2 - curRect->y1;
          double cy = dy/fabs(dy);
          double slope = dx/dy;
          
          double x = curRect->x1;
          double y = curRect->y1;
          
          for (double l=0.0; l<=fabs(dy); l+=1.0){
            newGrid->grid->setValue((int)x, (int)y, (char)-10);
            y += cy;
            x = (y - curRect->y1) * slope + curRect->x1;
          }
          
        } else {
          for (int k=0; k<curRegion->size; k++){
            lsdimage->setValue(curRegion->xVals[k],curRegion->yVals[k],curRegion->angle);
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
  unsigned char curAngle;
  
  int regSize;
  unsigned char regAngle;
  PointList* ptList = new PointList;
  PointList* curPt = ptList;
  PointList* iterPt = ptList;

  //first point of the region
  regSize = 1;
  ptList->x = x;
  ptList->y = y;
  ptList->value = this->lsdimage->map[y*LSD_GRID_SIZE + x];
  lsdimage->setValue(x,y,UNDEFINED);
  
  //angle of region
  regAngle = ptList->value;

  //try neighbors of existing region points as new points in region
  for(i=0; i<regSize; i++) {
    for (xx=(iterPt->x-1); xx<=(iterPt->x+1); xx++){
      for (yy=(iterPt->y-1); yy<=(iterPt->y+1); yy++){
        
//         dx = abs(x - xx);
//         dy = abs(y - yy);
//         if ((dx > 50) || (dy > 50)) continue;
        
        //get current pixel value
        curAngle = lsdimage->getValue(xx,yy);
        
        //current pixel is edge and part of region
        if (curAngle != UNDEFINED) {
          //add point to pointlist
          PointList* pt = new PointList;
          pt->x = xx;
          pt->y = yy;
          pt->value = curAngle;
          lsdimage->setValue(xx,yy,UNDEFINED);
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
  reg->angle = regAngle;
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


//returns whether or not two angles are aligned according to the set threshold
bool LsdLineFitter::isAligned(unsigned char angle1, unsigned char angle2){
  if (angle1 > angle2) return ALIGNED_THRESH_CHAR >= (angle1 - angle2);
  else return ALIGNED_THRESH_CHAR >= (angle2 - angle1);
}


bool LsdLineFitter::isAligned(double angle1, double angle2){
  double diff = fabs(angle1 - angle2);
  
  if (diff <= ALIGNED_THRESH_DOUBLE) return true;
  else if (diff >= (M_PI - ALIGNED_THRESH_DOUBLE)) return true;
  else return false;
}


double LsdLineFitter::getTheta(Region* reg, double x, double y){
  double lambda,theta;
  double Ixx = 0.0;
  double Iyy = 0.0;
  double Ixy = 0.0;
  int i;

  //compute inertia matrix
  for(i=0; i<reg->size; i++)
    {
      Ixx += ((double)reg->yVals[i] - y) * ((double)reg->yVals[i] - y);
      Iyy += ((double)reg->xVals[i] - x) * ((double)reg->xVals[i] - x);
      Ixy -= ((double)reg->xVals[i] - x) * ((double)reg->yVals[i] - y);
    }
    
  //compute smallest eigenvalue
  lambda = 0.5 * ( Ixx + Iyy - sqrt( (Ixx-Iyy)*(Ixx-Iyy) + 4.0*Ixy*Ixy ) );

  //compute angle
  theta = fabs(Ixx)>fabs(Iyy) ? atan2(lambda-Ixx,Ixy) : atan2(Ixy,lambda-Iyy);
  
  //make positive
  if (theta < 0.0) theta += M_PI;
    
  return theta;
}


//distance between two points
double LsdLineFitter::dist(double x1, double y1, double x2, double y2){
  return sqrt((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1));
}


LsdLineFitter::Rect* LsdLineFitter::regionToRect(Region* reg){
  double x,y,dx,dy,l,w,theta,l_min,l_max,w_min,w_max;
  int i;

  //center of region using LS
  x = 0;
  y = 0;
  for (i=0; i<reg->size; i++){
    x += reg->xVals[i];
    y += reg->yVals[i];
  }
  x = x / (double)reg->size;
  y = y / (double)reg->size;

  //angle of region
  theta = getTheta(reg, x, y);

  /* length and width:

     'l' and 'w' are computed as the distance from the center of the
     region to pixel i, projected along the rectangle axis (dx,dy) and
     to the orthogonal axis (-dy,dx), respectively.

     The length of the rectangle goes from l_min to l_max, where l_min
     and l_max are the minimum and maximum values of l in the region.
     Analogously, the width is selected from w_min to w_max, where
     w_min and w_max are the minimum and maximum of w for the pixels
     in the region.
   */
  dx = cos(theta);
  dy = sin(theta);
  l_min = l_max = w_min = w_max = 0.0;
  for(i=0; i<reg->size; i++)
    {
      l =  ((double)reg->xVals[i] - x) * dx + ((double)reg->yVals[i] - y) * dy;
      w = -((double)reg->xVals[i] - x) * dy + ((double)reg->yVals[i] - y) * dx;

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
  if( rec->width < 1.0 ) rec->width = 1.0;
  
  return rec;
}


bool LsdLineFitter::refineRect(Rect* rec, Region* reg){
  double density,xc,yc;
  double rad1,rad2,rad;
  int i;
  Rect* newRect;

  //compute region points density
  density = (double)reg->size / (dist(rec->x1,rec->y1,rec->x2,rec->y2) * rec->width);

  //if the density criterion is satisfied there is nothing to do
  if( density >= DENSITY_TH ) return true;

  //-----------------Try reducing region radius----------------
  //compute region's radius
  xc = (double) reg->xVals[0];
  yc = (double) reg->yVals[0];
  rad1 = dist( xc, yc, rec->x1, rec->y1 );
  rad2 = dist( xc, yc, rec->x2, rec->y2 );
  rad = rad1 > rad2 ? rad1 : rad2;
  
  //while the density criterion is not satisfied, remove farther pixels
  while( density < DENSITY_TH ) {
    rad *= 0.75; //reduce region's radius to 75% of its value

    //remove points from the region and update 'used' map
    for(i=0; i<reg->size; i++)
      if(dist(xc, yc, (double)reg->xVals[i], (double)reg->yVals[i]) > rad)
        {
          //point not kept, mark it as unused
          lsdimage->setValue(reg->xVals[i],reg->yVals[i],reg->angle);
          
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
    density = (double)reg->size / (dist(rec->x1,rec->y1,rec->x2,rec->y2) * rec->width);
  }

  //if this point is reached, the density criterion is satisfied
  return true;
}



void LsdLineFitter::rectImprove(){
  
}



int THRESH = 1;


void LsdLineFitter::blurImageX(){
  for (int i=0; i<LSD_GRID_SIZE; i++){
    for (int j=0; j<LSD_GRID_SIZE; j++){
      
      if (this->lsdimage->map[i*LSD_GRID_SIZE + j] != LsdGrid::UNDEFINED){
        for (int k=j-THRESH; k<=j+THRESH; k++){
          this->lsdimage->setValue(k, i, 1);
        }
        j += THRESH;
      }
      
    }
  }
}


void LsdLineFitter::blurImageY(){
  for (int i=0; i<LSD_GRID_SIZE; i++){
    for (int j=0; j<LSD_GRID_SIZE; j++){
      
      if (this->lsdimage->map[j*LSD_GRID_SIZE + i] != LsdGrid::UNDEFINED){
        for (int k=j-THRESH; k<=j+THRESH; k++){
          this->lsdimage->setValue(i, k, 1);
        }
        j += THRESH;
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

