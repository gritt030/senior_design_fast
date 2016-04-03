#include <iostream>
#include <sys/time.h>
#include <cstdlib>
#include <cmath>
#include <algorithm>
#include <string.h>
#include <cstdio>
#include <ctime>


#include "occupancygrid/grid/grid.h"
#include "occupancygrid/occupancygrid.h"
#include "localization/coordinatereader.h"
#include "localization/localizer.h"
#include "navigation/virtualenvironment.h"
#include "sonar/sonararchive.h"
#include "navigation/navigationmap/navigationmap.h"
#include "linefitter/lsdlinefitter.h"
#include "linefitter/houghtransform.h"

#include "pngwriter/png_writer.h"


int main(int argc, char **argv) {
  if (argc != 2) {std::cout << "More args please!\n"; return 0;}
  
  char* occImg = "/home/owner/pics/pics/occupancy.png";
  char* sorImg = "/home/owner/pics/pics/refined.png";
  char* rawImg = "/home/owner/pics/pics/rawnav.png";
  char* navImg = "/home/owner/pics/pics/navigate.png";
  //char* coordFile = "/home/owner/workspace/Datasets/output_ds3/coordsEstimate.txt";
  char* coordFile = (char*)argv[1];
  
  CoordinateReader* r = new CoordinateReader(coordFile);
  
  r->updateCoordsFile();  //use file for maps
  
  /*//use virtual environment for maps
  r->updateCoordsVirtual(buf1);
  //*/
  
  Localizer* l = new Localizer(r);
  
  NavigationMap* g = new NavigationMap();
  //OccupancyGrid* g = new OccupancyGrid();
  
  SonarArchive* a = new SonarArchive();
  
  int* drone = new int[3];
  int* weson = new int[3];
  int* eason = new int[3];
  int* nwson = new int[3];
  int* neson = new int[3];
  bool* range = new bool[4];
  float angle = 0.17;
  double heading = 0.0;
  double distX = 0.0;
  double distY = 0.0;
  double distA = 0.0;
  double* prevLoc = new double[5];
  double prevAngle = 0.0;
  int* sonarDists = new int[4];
  double* rawPos = new double[5];
  
  double XDrift = 0.0;
  double YDrift = 0.0;
  
  int index = 0;
  char* name = new char[256];
  
  //std::srand(std::time(nullptr));
  int num = 0;
  
  /////Normal main loop /////
  //for (int i=0; i<750; i++) r->updateCoordsFile();
  for (int i=0; i<3000; i++){
    //std::cout << "---- " << i << " ----" << std::endl;
    l->triggerUpdate();
    
    l->getPosition(drone);
    l->getWSonarPosition(weson);
    l->getESonarPosition(eason);
    l->getNWSonarPosition(nwson);
    l->getNESonarPosition(neson);
    l->getSonarInRange(range);
    
    l->getRawSonarDists(sonarDists);
    l->getRawPosition(rawPos);
    distX += sqrt((rawPos[0]-prevLoc[0])*(rawPos[0]-prevLoc[0]));
    distY += sqrt((rawPos[1]-prevLoc[1])*(rawPos[1]-prevLoc[1]));
    prevLoc[0] = rawPos[0];
    prevLoc[1] = rawPos[1];
    
    double angle1 = atan2(nwson[1]-drone[1], nwson[0]-drone[0]);
    double angle2 = atan2(neson[1]-drone[1], neson[0]-drone[0]);
    heading = (angle1 + angle2)/2.0;
    if (prevAngle == 0.0) prevAngle = heading;
    distA += sqrt((heading-prevAngle)*(heading-prevAngle));
    //angle = distA/1000.0 + 0.25;    //0.25
    angle = 0.17;
    angle = 0.27;
    prevAngle = heading;
    
    a->addSonarScan(sonarDists, rawPos[0], rawPos[1], distX/100.0, distY/100.0, heading, angle);
    
    r->updateCoordsFile(); //*/
    
  }
  
  std::cout << "Generating occupancy grid..." << std::endl;
  OccupancyGrid* orig = a->generateMap();
  //orig->sendToImage(navImg, 0,0);
  
  HoughTransform* hough = new HoughTransform(orig);
  double rotation = hough->getYCardinal();
  std::cout << "Y_Cardinal: " << rotation;
  std::cout << ", X_Cardinal: " << hough->getXCardinal() << std::endl;
  
  a->rotateMap(rotation);
  
  delete orig;
  orig = a->generateMap();
  //orig->sendToImage(sorImg, 0,0);
  
  
  OccupancyGrid* o1 = new OccupancyGrid();
  orig->getWallMap(o1);
  o1->blurMapX(5);
  o1->blurMapY(5);
  
  OccupancyGrid* o2 = new OccupancyGrid();
  orig->getOpenMap(o2);
  std::cout << "Begin LSD...\n";
  LsdLineFitter* lsd = new LsdLineFitter();
  lsd->detectLineSegmentsX(o1, o2);
  
//   delete o1;
//   o1 = new OccupancyGrid();
//   orig->getWallMap(o1);
//   
//   //o1->blurMapX(3);
//   //o1->blurMapY(5);
//   lsd->detectLineSegmentsY(o1, o2);
  std::cout << "End LSD\n";

  std::cout << "Image" << std::endl;
  std::cout << "Done!" << std::endl;

  
  return 0; //*/
  
}
