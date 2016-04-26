
#include "sonararchive.h"
#define HALF_PI 1.570796327

//constructor
SonarArchive::SonarArchive(){
  SonarScan* first = new SonarScan;
  first->w = 0;
  first->nw = 0;
  first->ne = 0;
  first->e = 0;
  first->x = 0.0f;
  first->y = 0.0f;
  first->heading = 0.0f;
  first->next = nullptr;
  
  this->scanList = first;
  this->lastScan = first;
}

SonarArchive::~SonarArchive(){
  SonarScan* cur;
  while (this->scanList != nullptr){
    cur = this->scanList;
    this->scanList = this->scanList->next;
    delete cur;
  }
}


void SonarArchive::addSonarScan(int* sonarDists, float x, float y, float head){
  //create sonar scan
  SonarScan* scan = new SonarScan;
  scan->w = sonarDists[0];
  scan->nw = sonarDists[1];
  scan->ne = sonarDists[2];
  scan->e = sonarDists[3];
  scan->x = x;
  scan->y = y;
  scan->rotX = x;
  scan->rotY = y;
  scan->heading = head;
  scan->rotHeading = head;
  scan->next = nullptr;
  
  //add sonar scan to list
  this->lastScan->next = scan;
  this->lastScan = scan;
}


void SonarArchive::addPath(OccupancyGrid* grid){
  SonarScan* cur = this->scanList->next;
  int x, y;
  
  while (cur != nullptr) {
    x = (int)(cur->rotX/SCALE);
    y = (int)(cur->rotY/SCALE);
    
    x += grid->BOUNDARY;
    y = grid->BOUNDARY - y;
    
    grid->grid->setValue(x, y, -grid->grid->MAX_VALUE);
    grid->grid->setValue(x+1, y, -grid->grid->MAX_VALUE);
    grid->grid->setValue(x, y+1, -grid->grid->MAX_VALUE);
    grid->grid->setValue(x-1, y, -grid->grid->MAX_VALUE);
    grid->grid->setValue(x, y-1, -grid->grid->MAX_VALUE);
    
    cur = cur->next;
  }
}


void SonarArchive::rotateMap(float angle){
  SonarScan* cur = this->scanList->next;
  float nX, nY;
  float co = cos(angle);
  float si = sin(angle);
  
  while (cur != nullptr){
    cur->rotHeading = cur->heading + angle;
    nX = (cur->x)*co - (cur->y)*si;
    nY = (cur->x)*si + (cur->y)*co;
    cur->rotX = nX;
    cur->rotY = nY;
    
    cur = cur->next;
  }
}


OccupancyGrid* SonarArchive::generateMap(float sliceAngle){  
  OccupancyGrid* output = new OccupancyGrid();
  int* buf = new int[8];
  int x, y;
  
  SonarScan* current = this->scanList->next;
  
  while (current != nullptr){  
    x = (int)(current->rotX/SCALE);
    y = (int)(current->rotY/SCALE);
    
    this->getSonarCoords(current, buf);
    
    if (current->w < SONAR_MAX) output->closeSlice(x, y, x+buf[0], y+buf[1], sliceAngle);
    else output->openSlice(x, y, x+buf[0], y+buf[1], sliceAngle);
    
    if (current->e < SONAR_MAX) output->closeSlice(x, y, x+buf[6], y+buf[7], sliceAngle);
    else output->openSlice(x, y, x+buf[6], y+buf[7], sliceAngle);
    
//     //TODO: Front sonars, don't use
//     if (current->nw < SONAR_MAX) output->closeSlice(x, y, x+buf[2], y+buf[3], sliceAngle);
//     else output->openSlice(x, y, x+buf[2], y+buf[3], sliceAngle);
//     
//     if (current->ne < SONAR_MAX) output->closeSlice(x, y, x+buf[4], y+buf[5], sliceAngle);
//     else output->openSlice(x, y, x+buf[4], y+buf[5], sliceAngle);

    current = current->next;
  }

  delete[] buf;
  return output;
}


void SonarArchive::getSonarCoords(SonarScan* scan, int* buffer){
  float Angle = scan->rotHeading;
  float co = cos(Angle);
  float si = sin(Angle);
  float cop = cos(Angle + HALF_PI);
  float sip = sin(Angle + HALF_PI);
  float com = cos(Angle - HALF_PI);
  float sim = sin(Angle - HALF_PI);
  
  float x = (float)scan->w + 10.0f;
  float y = 0.0f;
  buffer[0] = (int)round(x*cop - y*sip);
  buffer[1] = (int)round(x*sip + y*cop);
  
  x = (float)scan->nw + 10.0f;
  y = 10.0f;
  buffer[2] = (int)round(x*co - y*si);
  buffer[3] = (int)round(x*si + y*co);
  
  x = (float)scan->ne + 10.0f;
  y = -10.0f;
  buffer[4] = (int)round(x*co - y*si);
  buffer[5] = (int)round(x*si + y*co);
  
  x = (float)scan->e + 10.0f;
  y = 0.0f;
  buffer[6] = (int)round(x*com - y*sim);
  buffer[7] = (int)round(x*sim + y*com);
  
  buffer[0] /= SCALE;
  buffer[1] /= SCALE;
  buffer[2] /= SCALE;
  buffer[3] /= SCALE;
  buffer[4] /= SCALE;
  buffer[5] /= SCALE;
  buffer[6] /= SCALE;
  buffer[7] /= SCALE;
}

