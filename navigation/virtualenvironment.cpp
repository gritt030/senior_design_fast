
#include "virtualenvironment.h"

#define PI 3.14159265359
#define HPI 1.57079632679

VirtualEnvironment::VirtualEnvironment() {
  this->map = new char[mapSize * mapSize]();
  
  loadHallway(10,10, 4990,210);
  loadHallway(10,10, 210,4990);
  loadHallway(4790,10, 4990,4990);
  loadHallway(10,4790, 4990,4990);
  loadHallway(10,2400, 4990,2600);
  loadHallway(2400,10, 2600,4990);
}


void VirtualEnvironment::loadHallway(int x1, int y1, int x2, int y2) {
  for (int i=x1; i<=x2; i++){
    for (int j=y1; j<=y2; j++){
      map[i*mapSize + j] = 1;
    }
  }
}


void VirtualEnvironment::setPosition(int x, int y){
  if ((x >= 0) && (y >= 0) && (x<mapSize) && (y<mapSize)){
    this->X = x;
    this->Y = y;
  }
}

void VirtualEnvironment::setRotation(float angle){
  this->Degrees = angle;
  
  while (Degrees >= 360.0f){
    Degrees -= 360.0f;
  }
  
  while (Degrees < 0.0f){
    Degrees += 360.0f;
  }
  
  Angle = Degrees*PI/180.0;
}


void VirtualEnvironment::changePosition(int dx, int dy) {
  if ((X + dx >= 0) && (Y + dy >= 0) && (X + dx<mapSize) && (Y + dy<mapSize)){
    this->X += dx;
    this->Y += dy;
  }
}


void VirtualEnvironment::changeRotation(float dangle) {
  this->Degrees += dangle;
  
  while (Degrees >= 360.0f){
    Degrees -= 360.0f;
  }
  
  while (Degrees < 0.0f){
    Degrees += 360.0f;
  }
  
  Angle = Degrees*PI/180.0;
};
    

void VirtualEnvironment::getPosition(int* buffer){
  buffer[0] = X;
  buffer[1] = Y;
}

//wx,wy,nwx,nwy,...
void VirtualEnvironment::getSonarCoords(int* buffer) {
  int* buf = new int[4];
  getSonarRanges(buf);
  
  double x = (double)buf[0] + 10.0;
  double y = 0.0;
  buffer[0] = (int)round(x*cos(Angle+HPI) - y*sin(Angle+HPI));
  buffer[1] = (int)round(x*sin(Angle+HPI) + y*cos(Angle+HPI));
  
  x = (double)buf[1] + 10.0;
  y = 10.0;
  buffer[2] = (int)round(x*cos(Angle) - y*sin(Angle));
  buffer[3] = (int)round(x*sin(Angle) + y*cos(Angle));
  
  x = (double)buf[2] + 10.0;
  y = -10.0;
  buffer[4] = (int)round(x*cos(Angle) - y*sin(Angle));
  buffer[5] = (int)round(x*sin(Angle) + y*cos(Angle));
  
  x = (double)buf[3] + 10.0;
  y = 0.0;
  buffer[6] = (int)round(x*cos(Angle-HPI) - y*sin(Angle-HPI));
  buffer[7] = (int)round(x*sin(Angle-HPI) + y*cos(Angle-HPI));
}


//w,nw,ne,e
void VirtualEnvironment::getSonarRanges(int* buffer) {
  buffer[0] = getWeSonar();
  buffer[1] = getNWSonar();
  buffer[2] = getNESonar();
  buffer[3] = getEaSonar();
}


int VirtualEnvironment::getNWSonar() {
  double dX = cos(Angle);
  double dY = sin(Angle);
  
  double startX = dX*10.0 - dY*10.0;
  double startY = dY*10.0 + dX*10.0;
  startX += (double)X;
  startY += (double)Y;
  
  double endX = startX;
  double endY = startY;
  
  while(map[(int)(endX * mapSize + endY)] != 0){
    endX += dX;
    endY += dY;
  }
  
  double deltaX = endX - startX;
  double deltaY = endY - startY;
  int dist = (int)round(sqrt(deltaX*deltaX + deltaY*deltaY));
  if(dist > 400) return 470;
  else return dist;
}


int VirtualEnvironment::getNESonar() {
  double dX = cos(Angle);
  double dY = sin(Angle);
  
  double startX = dX*10.0 + dY*10.0;
  double startY = dY*10.0 - dX*10.0;
  startX += (double)X;
  startY += (double)Y;
  
  double endX = startX;
  double endY = startY;
  
  while(map[(int)(endX * mapSize + endY)] != 0){
    endX += dX;
    endY += dY;
  }
  
  double deltaX = endX - startX;
  double deltaY = endY - startY;
  int dist = (int)round(sqrt(deltaX*deltaX + deltaY*deltaY));
  if(dist > 400) return 470;
  else return dist;
}


int VirtualEnvironment::getWeSonar() {
  double dX = cos(Angle + HPI);
  double dY = sin(Angle + HPI);
  
  double startX = dX*10.0 - dY*0.0;
  double startY = dY*10.0 + dX*0.0;
  startX += (double)X;
  startY += (double)Y;
  
  double endX = startX;
  double endY = startY;
  
  while(map[(int)(endX * mapSize + endY)] != 0){
    endX += dX;
    endY += dY;
  }
  
  double deltaX = endX - startX;
  double deltaY = endY - startY;
  int dist = (int)round(sqrt(deltaX*deltaX + deltaY*deltaY));
  if(dist > 400) return 470;
  else return dist;
}


int VirtualEnvironment::getEaSonar() {
  double dX = cos(Angle - HPI);
  double dY = sin(Angle - HPI);
  
  double startX = dX*10.0 - dY*0.0;
  double startY = dY*10.0 + dX*0.0;
  startX += (double)X;
  startY += (double)Y;
  
  double endX = startX;
  double endY = startY;
  
  while(map[(int)(endX * mapSize + endY)] != 0){
    endX += dX;
    endY += dY;
  }
    
  double deltaX = endX - startX;
  double deltaY = endY - startY;
  int dist = (int)round(sqrt(deltaX*deltaX + deltaY*deltaY));
  if(dist > 400) return 470;
  else return dist;
}


void VirtualEnvironment::getCurrentCoordinates(int* buffer){
  int* buf = new int[8]; 
  getSonarCoords(buf);
  
  buffer[0] = X - mapSize/2;
  buffer[1] = Y - mapSize/2;
  buffer[2] = 0;
  buffer[3] = buf[2];
  buffer[4] = buf[3];
  buffer[5] = 0;
  buffer[6] = buf[4];
  buffer[7] = buf[5];
  buffer[8] = 0;
  buffer[9] = buf[0];
  buffer[10] = buf[1];
  buffer[11] = 0;
  buffer[12] = buf[6];
  buffer[13] = buf[7];
  buffer[14] = 0;
  
  /*
  for (int i=0; i<15; i++)
    std::cout << buffer[i] << ", ";
  std::cout << std::endl;//*/
}