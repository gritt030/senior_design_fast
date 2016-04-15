#ifndef LSDLINEFITTER_H
#define LSDLINEFITTER_H

#include <cmath>
#include <chrono>

#include "./../ppmwriter/ppm_writer.h"
#include "./../occupancygrid/occupancygrid.h"
#include "./../occupancygrid/grid/grid.h"
#include "grid/lsdgrid.h"

class LsdLineFitter
{
public:
  //image pointers
  Grid* inimage = nullptr;
  Grid* outimage = nullptr;
  int LSD_GRID_SIZE;
  int MAX_X, MAX_Y, MIN_X, MIN_Y;
  
  static const unsigned char UNDEFINED = LsdGrid::UNDEFINED;
  static const unsigned char DEFINED = LsdGrid::DEFINED;
  static const int LENGTH = 10;
  static const int BLUR = 1;
  
  //detection parameters
  float SCALE = 0.6f; //sampling scale 0.8
  float SIGMA_SCALE = 0.6f; //sigma of gaussian filter 0.6
  float ANG_TH = 22.5f;  //22.5 0 < x < 180 diffs in angles less than this value are aligned
  float LOG_EPS = 0.0f; //0.0
  float DENSITY_TH = 0.7f; //0.7 0 <= x <= 1
  
  //detected lines
  int num_lines;
  float* detected_lines;
  
  //constructor
  LsdLineFitter();
  //destructor
  ~LsdLineFitter();
  
  void setImage(Grid* input);
  
  void detectLineSegments(OccupancyGrid* grid, OccupancyGrid* newGrid);
  void detectLineSegmentsX(OccupancyGrid* grid, OccupancyGrid* newGrid);
  void detectLineSegmentsY(OccupancyGrid* grid, OccupancyGrid* newGrid);
  
  void sendLsdToImage(char* filename);
  void setLsdImagePixel(PPMwriter* w, unsigned char value);
  
  
private:
  typedef struct {
    int size;
    int* xVals;
    int* yVals;
  } Region;
  
  typedef struct node {
    int x;
    int y;
    struct node* next;
  } PointList;
  
  typedef struct {
    float x1,y1,x2,y2; //first and second pts of line segment
    float width;       //rectangle width
    float x, y;        //center of rectangle
    float theta;       //angle
    float dx,dy;       //vector oriented as line segment
  } Rect;
  
  
private:
  LsdGrid* lsdimage = nullptr;
  
  unsigned char ALIGNED_THRESH_CHAR;
  float ALIGNED_THRESH_FLOAT;
  int MIN_REG_SIZE;
  
  void generateLsdImage();
  void crosshatchLsdImage();
  
  Region* regionGrow(int x, int y);
  bool isAligned(float angle1, float angle2);
  float getTheta(Region* reg, float x, float y);
  Rect* regionToRect(Region* reg);
  bool refineRect(Rect* rec, Region* reg);
  
  void blurImageX();
  void blurImageY();
  
  void deleteRegion(Region* reg);
  float dist(float x1, float y1, float x2, float y2);
};

#endif // LSDLINEFITTER_H

