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
  
//   this->sendLsdToImage("/home/owner/pics/pics/lsdmake.ppm");
  this->blurImageX();
  this->blurImageY();
  crosshatchLsdImage();
//   this->sendLsdToImage("/home/owner/pics/pics/lsdblur.ppm");
}


void LsdLineFitter::crosshatchLsdImage(){
  for (int i=0; i<LSD_GRID_SIZE; i++){
    for (int j=(i%HATCH); j<LSD_GRID_SIZE; j+=HATCH){
      lsdimage->setValue(j,i, UNDEFINED);
    }
  }
  for (int i=1; i<LSD_GRID_SIZE; i++){
    for (int j=((i-1)%HATCH); j<LSD_GRID_SIZE; j+=HATCH){
      lsdimage->setValue(j,i, UNDEFINED);
    }
  }
  
  for (int i=LSD_GRID_SIZE-1; i>=0; i--){
    for (int j=LSD_GRID_SIZE-(i%HATCH); j>=0; j-=HATCH){
      lsdimage->setValue(j,i, UNDEFINED);
    }
  }
  for (int i=LSD_GRID_SIZE-2; i>=0; i--){
    for (int j=LSD_GRID_SIZE-((i-1)%HATCH); j>=0; j-=HATCH){
      lsdimage->setValue(j,i, UNDEFINED);
    }
  }
//   this->sendLsdToImage("/home/owner/pics/pics/lsdcross.ppm");
}


void LsdLineFitter::detectLineSegments(OccupancyGrid* grid, OccupancyGrid* newGrid){
  this->inimage = grid->grid;
  
  this->generateLsdImage();
  //this->sendLsdToImage("/home/owner/pics/pics/lsd.ppm");
  
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
        
        //reject small regions
        if (curRegion->size < MIN_REG_SIZE) continue;

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



LsdLineFitter::Rect* LsdLineFitter::regionToRect(Region* reg){
  float x,y,theta;
  int i;
  
  //compute needed sums
  int sumX=0, sumY=0;
  for(i=0; i<reg->size; i++)
  {
    sumX += reg->xVals[i];
    sumY += reg->yVals[i];
  }

  
  //center of region using LS
  x = (float)sumX / (float)reg->size;
  y = (float)sumY / (float)reg->size;

  
  //find angle of region
  float Ixx = 0.0f;
  float Iyy = 0.0f;
  float Ixy = 0.0f;

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
  theta = 0.5 * fast_atan2(-2*Ixy, -Iyy + Ixx);
  theta += M_HPI;
  
  
  //integer approx
  int ic = (int)round(10000.0f * cos(theta));
  int is = (int)round(10000.0f * sin(theta));
  int ix = (int)round(x);
  int iy = (int)round(y);
  
  //length variables
  int maxL = (reg->xVals[0] - ix)*ic + (reg->yVals[0] - iy)*is;
  int minL = (reg->xVals[0] - ix)*ic + (reg->yVals[0] - iy)*is;
  int maxW = -(reg->xVals[0] - ix)*is + (reg->yVals[0] - iy)*ic;
  int minW = -(reg->xVals[0] - ix)*is + (reg->yVals[0] - iy)*ic;
  
  //find max/min length/width
  int curL, curW;
  for(i=1; i<reg->size; i++)
  {
    curL = (reg->xVals[i] - ix)*ic + (reg->yVals[i] - iy)*is;
    curW = -(reg->xVals[i] - ix)*is + (reg->yVals[i] - iy)*ic;
    
    if(curL > maxL) maxL = curL;
    else if(curL < minL) minL = curL;
    if(curW > maxW) maxW = curW;
    else if(curW < minW) minW = curW;
  }
  
  //calculate non-integer values
  float l_max = maxL / 10000.0f;
  float l_min = minL / 10000.0f;
  float w_max = maxW / 10000.0f;
  float w_min = minW / 10000.0f;
  float co = cos(theta);
  float si = sin(theta);
  
  
  //store values
  Rect* rec = new Rect;
  rec->x1 = x + l_min * co;
  rec->y1 = y + l_min * si;
  rec->x2 = x + l_max * co;
  rec->y2 = y + l_max * si;
  rec->width = w_max - w_min;
  rec->x = x;
  rec->y = y;
  rec->theta = theta;
  rec->dx = co;
  rec->dy = si;

  //we impose a minimal width of one pixel
  if( rec->width < 1.0f ) rec->width = 1.0f;
  
  return rec;
}



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


float LsdLineFitter::fast_atan2(float y, float x){
  float t, val;
  if ((fabs(x) < 0.000001f) && (fabs(y) < 0.000001f)) return 0.0f;
  else if ( x == 0.0f ) t = 1.570796327f;
  else {
    val = fabs(y/x);
  
    if (val < 1.0f){
      t = val/(1.0f + 0.28f*val*val);
    } else {
      t = 1.570796327f - val/(val*val + 0.28f);
    }
  }
  
  if (x>=0) {
    if (y<0) t = -t;
  }
  else if (y<0) t = t-3.14192654;
  else t = 3.14192654-t;
  return t;
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

