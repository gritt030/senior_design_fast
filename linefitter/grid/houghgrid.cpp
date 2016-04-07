
#include "houghgrid.h"

//constructor, create grid
HoughGrid::HoughGrid() {
  this->map = new unsigned char[RADIUS_SIZE * THETA_SIZE]();
  this->D_THETA = 3.141592654 / THETA_SIZE;
  
  SIN_ARRAY = new float[THETA_SIZE];
  COS_ARRAY = new float[THETA_SIZE];
  
  double theta = 0.0;
  for (int i=0; i<THETA_SIZE; i++){
    SIN_ARRAY[i] = (float)sin(theta);
    COS_ARRAY[i] = (float)cos(theta);
    theta += D_THETA;
  }
}

//destructor, destroy grid
HoughGrid::~HoughGrid() {
  delete[] this->map;
}


// //add the hough transform of a point into the hough grid
// void HoughGrid::addHoughPoint(int x, int y){
//   double theta = 0.0;
//   Num_Points++;
//   int dx = x-CENTER;
//   int dy = y-CENTER;
//   int rad;
//   
//   for (int i=0; i<THETA_SIZE; i++) {
//     rad = (int)(dx*cos(theta) + dy*sin(theta));
//     rad += ADDITION;
//          
//     this->map[i*RADIUS_SIZE + (rad >> 3)] |= (0x01 << (rad % 8));
//     
//     theta += D_THETA;
//   }
// }



//add the hough transform of a point into the hough grid
void HoughGrid::addHoughPoint(int x, int y){
  Num_Points++;
  int dx = x-CENTER;
  int dy = y-CENTER;
  int rad;
  
  if (abs(dx) < abs(dy)) return;
  
  for (int i=0; i<THETA_SIZE; i++) {
    //rad = (int)(dx*SIN_ARRAY[(i+COS_OFFSET) % THETA_SIZE] + dy*SIN_ARRAY[i]);
    rad = (int)(dx*COS_ARRAY[i] + dy*SIN_ARRAY[i]);
    if (rad < 0) rad *= -1;
    rad += ADDITION;
         
    this->map[i*RADIUS_SIZE + (rad >> 3)] |= (0x01 << (rad % 8));    
  }
}


//sum the number of filled cells for each theta column
int HoughGrid::getThetaSums(int* sums){
  int curSum;
  unsigned char curByte;
  
  
  for (int i=0; i<THETA_SIZE; i++){
    curSum = 0;
    
    for (int j=0; j<RADIUS_SIZE; j++){
      curByte = this->map[i*RADIUS_SIZE + j];
      
      curSum += curByte & 0x01;
      curSum += (curByte >> 1) & 0x01;
      curSum += (curByte >> 2) & 0x01;
      curSum += (curByte >> 3) & 0x01;
      curSum += (curByte >> 4) & 0x01;
      curSum += (curByte >> 5) & 0x01;
      curSum += (curByte >> 6) & 0x01;
      curSum += (curByte >> 7) & 0x01;
    }
    
    sums[i] = curSum;
    std::cout << i << ", " << curSum << std::endl;
  }
  
  return this->Num_Points;
}




void HoughGrid::sendHoughToImage(char* filename){
  PPMwriter* w = new PPMwriter();
  unsigned char curByte;
  
  w->create_image(filename, RADIUS_SIZE*8, THETA_SIZE);
  
  for (int i=0; i<THETA_SIZE; i++){
    
    for (int j=0; j<RADIUS_SIZE; j++){
      curByte = this->map[i*RADIUS_SIZE + j];

      setImagePixel(w, curByte & 0x01);
      setImagePixel(w, (curByte >> 1) & 0x01);
      setImagePixel(w, (curByte >> 2) & 0x01);
      setImagePixel(w, (curByte >> 3) & 0x01);
      setImagePixel(w, (curByte >> 4) & 0x01);
      setImagePixel(w, (curByte >> 5) & 0x01);
      setImagePixel(w, (curByte >> 6) & 0x01);
      setImagePixel(w, (curByte >> 7) & 0x01);
    }
  }
  
  w->output_image();
  delete w;
}


void HoughGrid::setImagePixel(PPMwriter* w, int val){
  //open map square
  if (val > 0) {
    w->write_pixel(0x00, 0x88, 0x00);
  
  //unknown map square
  } else {
    w->write_pixel(0x80, 0x80, 0x80);
  }
}

