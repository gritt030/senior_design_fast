#include <iostream>
#include <sys/time.h>
#include <cstdlib>
#include <cmath>
#include <algorithm>
#include <string.h>
#include <cstdio>
#include <ctime>
#include <chrono>


#include "occupancygrid/grid/grid.h"
#include "occupancygrid/occupancygrid.h"
#include "localization/coordinatereader.h"
#include "localization/localizer.h"
#include "sonar/sonararchive.h"
#include "linefitter/lsdlinefitter.h"
#include "linefitter/houghtransform.h"
#include "ppmwriter/ppm_writer.h"


int main(int argc, char **argv) {  
  if (argc != 2) {std::cout << "More args please!\n"; return 0;}
  
  char* occImg = "/home/owner/pics/pics/occupancy.ppm";
  char* sorImg = "/home/owner/pics/pics/refined.ppm";
  char* rawImg = "/home/owner/pics/pics/rawnav.ppm";
  char* navImg = "/home/owner/pics/pics/navigate.ppm";
  //char* coordFile = "/home/owner/workspace/Datasets/output_ds3/coordsEstimate.txt";
  char* coordFile = (char*)argv[1];
  
  CoordinateReader* r = new CoordinateReader(coordFile);
  
  r->updateCoordsFile();  //use file for maps
  
  Localizer* l = new Localizer(r);
  
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
  
  long long t1 = 0;
  std::chrono::high_resolution_clock::time_point t1_1;
  std::chrono::high_resolution_clock::time_point t1_2;
  
  
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
    
    t1_1 = std::chrono::high_resolution_clock::now();
    a->addSonarScan(sonarDists, rawPos[0], rawPos[1], distX/100.0, distY/100.0, heading, angle);
    t1_2 = std::chrono::high_resolution_clock::now();
    t1 += std::chrono::duration_cast<std::chrono::nanoseconds>(t1_2-t1_1).count();
    
//     if (i%10 == 0) {
//       std::chrono::high_resolution_clock::time_point tloop_1 = std::chrono::high_resolution_clock::now();
//       OccupancyGrid* loopmap = a->generateMap();
//       std::chrono::high_resolution_clock::time_point tloop_2 = std::chrono::high_resolution_clock::now();
//       long long nano = std::chrono::duration_cast<std::chrono::nanoseconds>(tloop_2-tloop_1).count();
//       std::cout << nano << std::endl;
//       delete loopmap;
//     }
    
    r->updateCoordsFile(); //*/
    
  }
  
//   std::cout << "Bananan\n";
//   return 0;
  
  ///std::cout << "Generating occupancy grid..." << std::endl;
  std::chrono::high_resolution_clock::time_point t2_1 = std::chrono::high_resolution_clock::now();
  OccupancyGrid* orig = a->generateMap();
  std::chrono::high_resolution_clock::time_point t2_2 = std::chrono::high_resolution_clock::now();
  ////orig->sendToImage(navImg, 0,0);
  
  std::chrono::high_resolution_clock::time_point t3_1 = std::chrono::high_resolution_clock::now();
  HoughTransform* hough = new HoughTransform(orig);
  double rotation = hough->getYCardinal();
  std::chrono::high_resolution_clock::time_point t3_2 = std::chrono::high_resolution_clock::now();
  
  
//   std::cout << "Y_Cardinal: " << rotation;
//   std::cout << ", X_Cardinal: " << hough->getXCardinal() << std::endl;
//   a->rotateMap(rotation);
//   a->generateMap()->sendToImage(occImg, 0,0);
//   return 0;
  
  
  
  
  
  std::chrono::high_resolution_clock::time_point t4_1 = std::chrono::high_resolution_clock::now();
  a->rotateMap(rotation);
  std::chrono::high_resolution_clock::time_point t4_2 = std::chrono::high_resolution_clock::now();
  
  delete orig;
  std::chrono::high_resolution_clock::time_point t5_1 = std::chrono::high_resolution_clock::now();
  orig = a->generateMap();
  std::chrono::high_resolution_clock::time_point t5_2 = std::chrono::high_resolution_clock::now();
  ////orig->sendToImage(sorImg, 0,0);
  
  
  std::chrono::high_resolution_clock::time_point t6_1 = std::chrono::high_resolution_clock::now();
  OccupancyGrid* o1 = new OccupancyGrid();
  orig->getWallMap(o1);
//   o1->blurMapX(5);
//   o1->blurMapY(5);
  std::chrono::high_resolution_clock::time_point t6_2 = std::chrono::high_resolution_clock::now();
  
  OccupancyGrid* o2 = new OccupancyGrid();
  orig->getOpenMap(o2);
  ///std::cout << "Begin LSD...\n";
  
  std::chrono::high_resolution_clock::time_point t7_1 = std::chrono::high_resolution_clock::now();
  LsdLineFitter* lsd = new LsdLineFitter();
  //lsd->detectLineSegmentsX(o1, o2);
  lsd->detectLineSegments(o1, o2);
  std::chrono::high_resolution_clock::time_point t7_2 = std::chrono::high_resolution_clock::now();
  
//   delete o1;
//   o1 = new OccupancyGrid();
//   orig->getWallMap(o1);
//   
//   o1->blurMapX(3);
//   o1->blurMapY(5);
//   lsd->detectLineSegmentsY(o1, o2);
  ///std::cout << "End LSD\n";

  ///std::cout << "Image" << std::endl;
  std::chrono::high_resolution_clock::time_point t8_1 = std::chrono::high_resolution_clock::now();
  o2->sendToImage(occImg, 0,0);
  std::chrono::high_resolution_clock::time_point t8_2 = std::chrono::high_resolution_clock::now();
  ///std::cout << "Done!" << std::endl;



  
  std::cout << t1 << " ";
  long long nano = std::chrono::duration_cast<std::chrono::nanoseconds>(t2_2-t2_1).count();
  std::cout << nano << " ";
  nano = std::chrono::duration_cast<std::chrono::nanoseconds>(t3_2-t3_1).count();
  std::cout << nano << " ";
  nano = std::chrono::duration_cast<std::chrono::nanoseconds>(t4_2-t4_1).count();
  std::cout << nano << " ";
  nano = std::chrono::duration_cast<std::chrono::nanoseconds>(t5_2-t5_1).count();
  std::cout << nano << " ";
  nano = std::chrono::duration_cast<std::chrono::nanoseconds>(t6_2-t6_1).count();
  std::cout << nano << " ";
  nano = std::chrono::duration_cast<std::chrono::nanoseconds>(t7_2-t7_1).count();
  std::cout << nano << " ";
  nano = std::chrono::duration_cast<std::chrono::nanoseconds>(t8_2-t8_1).count();
  std::cout << nano << std::endl;
  
  return 0; //*/
  
}
