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
#include "sonar/sonararchive.h"
#include "linefitter/lsdlinefitter.h"
#include "linefitter/houghtransform.h"
#include "ppmwriter/ppm_writer.h"


int main(int argc, char **argv) {
  if (argc < 2) {std::cout << "First arg is input filename please\n"; return 0;}
  
  char* occImg = "/home/owner/pics/pics/occupancy.ppm";
  char* sorImg = "/home/owner/pics/pics/refined.ppm";
  char* navImg = "/home/owner/pics/pics/straightened.ppm";
//   char* occImg = "/data/ftp/internal_000/senior_design/images/occupancy.ppm";
//   char* sorImg = "/data/ftp/internal_000/senior_design/images/refined.ppm";
//   char* rawImg = "/data/ftp/internal_000/senior_design/images/rawnav.ppm";
//   char* navImg = "/data/ftp/internal_000/senior_design/images/straightened.ppm";
  char* coordFile = (char*)argv[1];
  char* outputFile = (char*)argv[2];
  
  //reads sonar/position data in from file
  CoordinateReader* r = new CoordinateReader(coordFile);
  r->updateCoordsFileNew();
  
  //linked list of saved sonar scans
  SonarArchive* a = new SonarArchive();
  
  //variables to save data in
  bool test;
  double* pose = new double[3];
  int* sonars = new int[4];
  
  
  //save all sonar scans from datafile
  //i=number of scans to try to read in
  for (int i=0; i<3000; i++){
    //read in data from file, break if end of file
    test = r->getCurrentPoseNew(pose);
    r->getCurrentSonarsNew(sonars);
    if (!test) break;
    
    ///////////////////////////////////////////////////////
    //w and e sonars are flipped on walter library datasets
    int w = sonars[0];
    sonars[0] = sonars[3];
    sonars[3] = w;
    //////////////////////////////////////////////////////
    
    //add scan to collection
    a->addSonarScan(sonars, pose[0], pose[1], pose[2]);
    
    //move to next line in file
    r->updateCoordsFileNew();
  }

  //generate map from saved sonar scans
  OccupancyGrid* orig = a->generateMap(0.27f);
  orig->sendToImage(occImg, 0,0);

  //perform hough transform on map, rotate saved sonar scans to align cardinal directions
  std::vector<int>* xPos = new std::vector<int>();
  std::vector<int>* yPos = new std::vector<int>();
  int numPts = orig->getWallMap(xPos, yPos);
  HoughTransform* hough = new HoughTransform(numPts, xPos, yPos);
  double rotation = hough->getYCardinal();
  delete xPos;
  delete yPos;
  
  //perform actual rotation
  a->rotateMap(rotation);
 
  //re-generate straightened map
  delete orig;
  orig = a->generateMap(0.27f);
  orig->sendToImage(navImg, 0,0);

  //get walls in map
  OccupancyGrid* o1 = new OccupancyGrid();
  orig->getWallMap(o1);
  
  //get open spaces in map
  OccupancyGrid* o2 = new OccupancyGrid();
  orig->getOpenMap(o2);
  
  //run lsd on wall map, save lines to open map
  LsdLineFitter* lsd = new LsdLineFitter();
  lsd->detectLineSegments(o1, o2);

  //send lsd map to image
  o2->sendToImage(sorImg, 0,0);
  
  //clean up everything
  delete o1;
  delete o2;
  delete orig;
  delete lsd;
  delete hough;
  delete a;
  
  std::cout << "Done!" << std::endl;
  return 0;
  
}
