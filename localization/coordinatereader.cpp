
#include "coordinatereader.h"

CoordinateReader::CoordinateReader(char* filename){
  this->coordFile.open(filename);
  this->currentCoords = new int[15];
  
  for (int i=0; i<15; i++){
    currentCoords[i] = 0;
  }
}


CoordinateReader::CoordinateReader(){
  this->currentCoords = new int[15];
  
  for (int i=0; i<15; i++){
    currentCoords[i] = 0;
  }
}


void CoordinateReader::updateCoordsFile(){  
  long time;
  double x, y, z;
  double nwx, nwy, nwz;
  double nex, ney, nez;
  double wx, wy, wz;
  double ex, ey, ez;
  
  coordFile >> time;
  coordFile >> x >> y >> z;
  coordFile >> nwx >> nwy >> nwz;
  coordFile >> nex >> ney >> nez;
  coordFile >> wx >> wy >> wz;
  coordFile >> ex >> ey >> ez;
  
  if (coordFile.eof()){
    for (int i=0; i<15; i++){
      currentCoords[i] = 0;
    }
    return;
  }
  
  double scale = 10.0;
  
  this->currentCoords[0] = round(x/scale);
  this->currentCoords[1] = round(y/scale);
  this->currentCoords[2] = round(z/scale);
  this->currentCoords[3] = round(nwx/scale);
  this->currentCoords[4] = round(nwy/scale);
  this->currentCoords[5] = round(nwz/scale);
  this->currentCoords[6] = round(nex/scale);
  this->currentCoords[7] = round(ney/scale);
  this->currentCoords[8] = round(nez/scale);
  this->currentCoords[9] = round(wx/scale);
  this->currentCoords[10] = round(wy/scale);
  this->currentCoords[11] = round(wz/scale);
  this->currentCoords[12] = round(ex/scale);
  this->currentCoords[13] = round(ey/scale);
  this->currentCoords[14] = round(ez/scale);
  /*
  std::cout << "New Pos: " << currentCoords[0] << ", ";
  std::cout << currentCoords[0] << ", " << currentCoords[1] << ", ";
  std::cout << currentCoords[3] << ", " << currentCoords[4] << ", ";
  std::cout << currentCoords[6] << ", " << currentCoords[7] << ", ";
  std::cout << currentCoords[9] << ", " << currentCoords[10] << ", ";
  std::cout << currentCoords[12] << ", " << currentCoords[13] << std::endl;
  //*/
}



void CoordinateReader::updateCoordsVirtual(int* coords){
  this->currentCoords[0] = round(coords[0]);
  this->currentCoords[1] = round(coords[1]);
  this->currentCoords[2] = round(coords[2]);
  this->currentCoords[3] = round(coords[3]);
  this->currentCoords[4] = round(coords[4]);
  this->currentCoords[5] = round(coords[5]);
  this->currentCoords[6] = round(coords[6]);
  this->currentCoords[7] = round(coords[7]);
  this->currentCoords[8] = round(coords[8]);
  this->currentCoords[9] = round(coords[9]);
  this->currentCoords[10] = round(coords[10]);
  this->currentCoords[11] = round(coords[11]);
  this->currentCoords[12] = round(coords[12]);
  this->currentCoords[13] = round(coords[13]);
  this->currentCoords[14] = round(coords[14]);
  
  /*
  for (int i=0; i<15; i++)
    std::cout << this->currentCoords[i] << ", ";
  std::cout << std::endl; //*/
}


//get position data
void CoordinateReader::getCurrentCoords(int* buffer){
  buffer[0] = this->currentCoords[0];
  buffer[1] = this->currentCoords[1];
  buffer[2] = this->currentCoords[2];
}
void CoordinateReader::getCurrentCoordError(int* buffer){
  buffer[0] = 0;
  buffer[1] = 0;
}
    
//get rotation data
void CoordinateReader::getCurrentRotation(float* buffer){
  buffer[0] = 0.0;
  buffer[1] = 0.0;
  buffer[2] = 0.0;
}
float CoordinateReader::getYawError(){
  return 0.0;
}
    
//get sonar data
void CoordinateReader::getCurrentSonarCoords(int* buffer){
  buffer[3] = this->currentCoords[3];
  buffer[4] = this->currentCoords[4];
  buffer[5] = this->currentCoords[5];
  buffer[6] = this->currentCoords[6];
  buffer[7] = this->currentCoords[7];
  buffer[8] = this->currentCoords[8];
  buffer[0] = this->currentCoords[9];
  buffer[1] = this->currentCoords[10];
  buffer[2] = this->currentCoords[11];
  buffer[9] = this->currentCoords[12];
  buffer[10] = this->currentCoords[13];
  buffer[11] = this->currentCoords[14];
}
void CoordinateReader::getCurrentSonarDists(int* buffer){
  int x = this->currentCoords[9];
  int y = this->currentCoords[10];
  int z = this->currentCoords[11];
  int d = (int)sqrt(x*x + y*y + z*z);
  buffer[0] = d;
  
  x = this->currentCoords[3];
  y = this->currentCoords[4];
  z = this->currentCoords[5];
  d = (int)sqrt(x*x + y*y + z*z);
  buffer[1] = d;
  
  x = this->currentCoords[6];
  y = this->currentCoords[7];
  z = this->currentCoords[8];
  d = (int)sqrt(x*x + y*y + z*z);
  buffer[2] = d;
  
  x = this->currentCoords[12];
  y = this->currentCoords[13];
  z = this->currentCoords[14];
  d = (int)sqrt(x*x + y*y + z*z);
  buffer[3] = d;
}



















