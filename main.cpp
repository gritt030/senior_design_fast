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
//   char* occImg = "/data/ftp/internal_000/senior_design/images/occupancy.ppm";
//   char* sorImg = "/data/ftp/internal_000/senior_design/images/refined.ppm";
//   char* rawImg = "/data/ftp/internal_000/senior_design/images/rawnav.ppm";
//   char* navImg = "/data/ftp/internal_000/senior_design/images/navigate.ppm";
  char* coordFile = (char*)argv[1];
  
  CoordinateReader* r = new CoordinateReader(coordFile);
  r->updateCoordsFileNew();
  
  SonarArchive* a = new SonarArchive();
  
  bool test;
  double* pose = new double[3];
  int* sonars = new int[4];
  
  int index = 0;
  char* name = new char[256];
  
  //std::srand(std::time(nullptr));
  int num = 0;
  
  long long t1 = 0;
  std::chrono::high_resolution_clock::time_point t1_1;
  std::chrono::high_resolution_clock::time_point t1_2;
  
  
  /////Normal main loop /////
//   for (int i=0; i<223; i++) r->updateCoordsFile();
  for (int i=0; i<3000; i++){
//     std::cout << "---- " << i << " ----" << std::endl;
    
    test = r->getCurrentPoseNew(pose);
    r->getCurrentSonarsNew(sonars);
    if (!test) break;
    
    t1_1 = std::chrono::high_resolution_clock::now();
    a->addSonarScan(sonars, pose[0], pose[1], pose[2]);
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
    
    r->updateCoordsFileNew();
  }
  
//   std::cout << "Bananan\n";
//   return 0;
  
  ///std::cout << "Generating occupancy grid..." << std::endl;
  std::chrono::high_resolution_clock::time_point t2_1 = std::chrono::high_resolution_clock::now();
  OccupancyGrid* orig = a->generateMap(0.27f);
  std::chrono::high_resolution_clock::time_point t2_2 = std::chrono::high_resolution_clock::now();
  ////orig->sendToImage(navImg, 0,0);
  
  std::chrono::high_resolution_clock::time_point t3_1 = std::chrono::high_resolution_clock::now();
  std::vector<int>* xPos = new std::vector<int>();
  std::vector<int>* yPos = new std::vector<int>();
  int numPts = orig->getWallMap(xPos, yPos);
  HoughTransform* hough = new HoughTransform(numPts, xPos, yPos);
//   HoughTransform* hough = new HoughTransform(orig);
  double rotation = hough->getYCardinal();
  delete xPos;
  delete yPos;
  std::chrono::high_resolution_clock::time_point t3_2 = std::chrono::high_resolution_clock::now();
    
/*
  char* fname = new char[128]();
  for (int i=1; i<=20; i+=1){
    hough->RANDOM_DIV = i;
    float rotation = hough->getYCardinal();
    std::cout << i << " : Y_Cardinal: " << rotation;
    std::cout << ", X_Cardinal: " << hough->getXCardinal() << std::endl;
    a->rotateMap(rotation);
    std::sprintf(fname, "/home/owner/pics/pics/sequence/rand%d.ppm", i);
    a->generateMap()->sendToImage(fname, 0,0);
    a->rotateMap(-rotation);
    delete hough;
    hough = new HoughTransform(numPts, xPos, yPos);
  }
  return 0;*/
  
//   for (int i=1; i<11; i++){
//     delete hough;
//     hough = new HoughTransform(numPts, xPos, yPos);
//     hough->RANDOM_DIV = i;
//     std::cout << i << " " << hough->getYCardinal() << std::endl;
//   }
//   return 0;
  
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
  orig = a->generateMap(0.27f);
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


//   //addscan genmap hough rotate genmap wallmap lsd image
//   std::cout << t1 << " ";
//   long long nano = std::chrono::duration_cast<std::chrono::nanoseconds>(t2_2-t2_1).count();
//   std::cout << nano << " ";
//   nano = std::chrono::duration_cast<std::chrono::nanoseconds>(t3_2-t3_1).count();
//   std::cout << nano << " ";
//   nano = std::chrono::duration_cast<std::chrono::nanoseconds>(t4_2-t4_1).count();
//   std::cout << nano << " ";
//   nano = std::chrono::duration_cast<std::chrono::nanoseconds>(t5_2-t5_1).count();
//   std::cout << nano << " ";
//   nano = std::chrono::duration_cast<std::chrono::nanoseconds>(t6_2-t6_1).count();
//   std::cout << nano << " ";
//   nano = std::chrono::duration_cast<std::chrono::nanoseconds>(t7_2-t7_1).count();
//   std::cout << nano << " ";
//   nano = std::chrono::duration_cast<std::chrono::nanoseconds>(t8_2-t8_1).count();
//   std::cout << nano << std::endl;
  
  return 0; //*/
  
}
