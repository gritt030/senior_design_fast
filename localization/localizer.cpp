
#include "localizer.h"

#define IND_X 0
#define IND_Y 1
#define IND_Z 2
#define IND_X_ERR 3
#define IND_Y_ERR 4

#define IND_WX 0
#define IND_WY 1
#define IND_WZ 2
#define IND_NWX 3
#define IND_NWY 4
#define IND_NWZ 5
#define IND_NEX 6
#define IND_NEY 7
#define IND_NEZ 8
#define IND_EX 9
#define IND_EY 10
#define IND_EZ 11

#define IND_WD 0
#define IND_NWD 1
#define IND_NED 2
#define IND_ED 3

#define IND_ROLL 0
#define IND_PITCH 1
#define IND_YAW 2
#define IND_YAW_ERR 3

//constructor
Localizer::Localizer(CoordinateReader* r){
  this->reader = r;
  this->currentCoords = new int[5];
  this->currentSonarCoords = new int[12];
  this->currentSonarDists = new int[4];
  this->currentRotation = new float[4];
}


//fills buffer with x, y, z coordinates according to scale
void Localizer::getPosition(int* buffer){
  buffer[0] = this->currentCoords[IND_X]/Localizer::SCALE;
  buffer[1] = this->currentCoords[IND_Y]/Localizer::SCALE;
  buffer[2] = this->currentCoords[IND_Z]/Localizer::SCALE;
}


//fills buffer with x-error and y-error values according to scale
void Localizer::getPositionError(int* buffer){
  buffer[0] = this->currentCoords[IND_X_ERR]/Localizer::SCALE;
  buffer[1] = this->currentCoords[IND_Y_ERR]/Localizer::SCALE;
}


//returns the sonar global x,y,z according to scale
void Localizer::getWSonarPosition(int* buffer){
  buffer[0] = (this->currentSonarCoords[IND_WX] + this->currentCoords[IND_X])/Localizer::SCALE;
  buffer[1] = (this->currentSonarCoords[IND_WY] + this->currentCoords[IND_Y])/Localizer::SCALE;
  buffer[2] = (this->currentSonarCoords[IND_WZ] + this->currentCoords[IND_Z])/Localizer::SCALE;
}
void Localizer::getNWSonarPosition(int* buffer){
  buffer[0] = (this->currentSonarCoords[IND_NWX] + this->currentCoords[IND_X])/Localizer::SCALE;
  buffer[1] = (this->currentSonarCoords[IND_NWY] + this->currentCoords[IND_Y])/Localizer::SCALE;
  buffer[2] = (this->currentSonarCoords[IND_NWZ] + this->currentCoords[IND_Z])/Localizer::SCALE;
}
void Localizer::getNESonarPosition(int* buffer){
  buffer[0] = (this->currentSonarCoords[IND_NEX] + this->currentCoords[IND_X])/Localizer::SCALE;
  buffer[1] = (this->currentSonarCoords[IND_NEY] + this->currentCoords[IND_Y])/Localizer::SCALE;
  buffer[2] = (this->currentSonarCoords[IND_NEZ] + this->currentCoords[IND_Z])/Localizer::SCALE;
}
void Localizer::getESonarPosition(int* buffer){
  buffer[0] = (this->currentSonarCoords[IND_EX] + this->currentCoords[IND_X])/Localizer::SCALE;
  buffer[1] = (this->currentSonarCoords[IND_EY] + this->currentCoords[IND_Y])/Localizer::SCALE;
  buffer[2] = (this->currentSonarCoords[IND_EZ] + this->currentCoords[IND_Z])/Localizer::SCALE;
}


//fills buffer with true/false based on whether or not
  //sonar values are in range or maxed out.
  //order: west, northwest, northeast, east
void Localizer::getSonarInRange(bool* buffer){
  //west
  if (this->currentSonarDists[IND_WD] < Localizer::SONAR_MAX) {
    buffer[0] = true;
  } else {
    buffer[0] = false;
  }
    
  //northwest
  if (this->currentSonarDists[IND_NWD] < Localizer::SONAR_MAX) {
    buffer[1] = true;
  } else {
    buffer[1] = false;
  }
  
  //northeast
  if (this->currentSonarDists[IND_NED] < Localizer::SONAR_MAX) {
    buffer[2] = true;
  } else {
    buffer[2] = false;
  }
  
  //east
  if (this->currentSonarDists[IND_ED] < Localizer::SONAR_MAX) {
    buffer[3] = true;
  } else {
    buffer[3] = false;
  }
}


//trigger update of everything
void Localizer::triggerUpdate(){
  this->updateCoords();
  this->updateSonars();
  this->updateRotation();
}


//use coordinate reader to update position data
void Localizer::updateCoords(){
  int* all = new int[3];
  reader->getCurrentCoords(all);
  
  this->currentCoords[IND_X] = all[0];
  this->currentCoords[IND_Y] = all[1];
  this->currentCoords[IND_Z] = all[2];
  
  delete all;
  all = new int[2];
  reader->getCurrentCoordError(all);
  this->currentCoords[IND_X_ERR] = all[0];
  this->currentCoords[IND_Y_ERR] = all[1];
}


//use coordinate reader to update rotation data
void Localizer::updateRotation(){
  float* all = new float[3];
  reader->getCurrentRotation(all);
  
  this->currentRotation[IND_ROLL] = all[0];
  this->currentRotation[IND_PITCH] = all[1];
  this->currentRotation[IND_YAW] = all[2];
  delete all;
  
  this->currentRotation[IND_YAW_ERR] = reader->getYawError();
}


//use coordinate reader to update sonar data
void Localizer::updateSonars(){
  int* all = new int[12];
  reader->getCurrentSonarCoords(all);
  
  this->currentSonarCoords[IND_WX] = all[0];
  this->currentSonarCoords[IND_WY] = all[1];
  this->currentSonarCoords[IND_WZ] = all[2];
  this->currentSonarCoords[IND_NWX] = all[3];
  this->currentSonarCoords[IND_NWY] = all[4];
  this->currentSonarCoords[IND_NWZ] = all[5];
  this->currentSonarCoords[IND_NEX] = all[6];
  this->currentSonarCoords[IND_NEY] = all[7];
  this->currentSonarCoords[IND_NEZ] = all[8];
  this->currentSonarCoords[IND_EX] = all[9];
  this->currentSonarCoords[IND_EY] = all[10];
  this->currentSonarCoords[IND_EZ] = all[11];
  
  delete all;
  all = new int[4];
  reader->getCurrentSonarDists(all);
  this->currentSonarDists[IND_WD] = all[0];
  this->currentSonarDists[IND_NWD] = all[1];
  this->currentSonarDists[IND_NED] = all[2];
  this->currentSonarDists[IND_ED] = all[3];
}


//get raw data values with errors

//x, y, z, xerr, yerr, zerr
void Localizer::getRawPosition(double* buffer){
  buffer[0] = this->currentCoords[IND_X];
  buffer[1] = this->currentCoords[IND_Y];
  buffer[2] = this->currentCoords[IND_Z];
}


//TODO: fix this
//roll, pitch, yaw, yawerr
void Localizer::getRawHeading(double* buffer){

}


//w, nw, ne, e
void Localizer::getRawSonarDists(int* buffer){
  buffer[0] = this->currentSonarDists[IND_WD];
  buffer[1] = this->currentSonarDists[IND_NWD];
  buffer[2] = this->currentSonarDists[IND_NED];
  buffer[3] = this->currentSonarDists[IND_ED];
}