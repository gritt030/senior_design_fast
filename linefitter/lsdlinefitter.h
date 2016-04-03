#ifndef LSDLINEFITTER_H
#define LSDLINEFITTER_H

#include <cmath>

#include "./../pngwriter/png_writer.h"
#include "./../occupancygrid/occupancygrid.h"
#include "./../occupancygrid/grid/grid.h"
#include "grid/lsdgrid.h"

class LsdLineFitter
{
public:
  //image pointers
  Grid* inimage = nullptr;
  Grid* outimage = nullptr;
  
  static const unsigned char UNDEFINED = LsdGrid::UNDEFINED;
  static const int LENGTH = 10;
  
  //detection parameters
  double SCALE = 0.8; //sampling scale 0.8
  double SIGMA_SCALE = 0.6; //sigma of gaussian filter 0.6
  double ANG_TH = 22.5;  //22.5 0 < x < 180 diffs in angles less than this value are aligned
  double LOG_EPS = 0.0; //0.0
  double DENSITY_TH = 0.7; //0.7 0 <= x <= 1
  
  //detected lines
  int num_lines;
  double* detected_lines;
  
  //constructor
  LsdLineFitter();
  //destructor
  ~LsdLineFitter();
  
  void setImage(Grid* input);
  
  void detectLineSegmentsX(OccupancyGrid* grid, OccupancyGrid* newGrid);
  void detectLineSegmentsY(OccupancyGrid* grid, OccupancyGrid* newGrid);
  
  void sendLsdToImage(char* filename);
  void setLsdImagePixel(PngWriter* w, int x, int y, unsigned char value);
  
  
private:
  typedef struct {
    int size;
    unsigned char angle;
    int* xVals;
    int* yVals;
  } Region;
  
  typedef struct node {
    int x;
    int y;
    unsigned char value;
    struct node* next;
  } PointList;
  
  typedef struct {
    double x1,y1,x2,y2; //first and second pts of line segment
    double width;       //rectangle width
    double x, y;        //center of rectangle
    double theta;       //angle
    double dx,dy;       //vector oriented as line segment
  } Rect;
  
  
private:
  LsdGrid* lsdimage = nullptr;
  
  unsigned char ALIGNED_THRESH_CHAR;
  double ALIGNED_THRESH_DOUBLE;
  
  void generateLsdImage();
  void gradientImageX();
  void gradientImageY();
  
  Region* regionGrow(int x, int y);
  bool isAligned(unsigned char angle1, unsigned char angle2);
  bool isAligned(double angle1, double angle2);
  double getTheta(Region* reg, double x, double y);
  Rect* regionToRect(Region* reg);
  bool refineRect(Rect* rec, Region* reg);
  void rectImprove();
  
  void deleteRegion(Region* reg);
  double dist(double x1, double y1, double x2, double y2);
  double nfa(int pts, int alg, double p, double logNT);
};

#endif // LSDLINEFITTER_H

/*

    @param n_bins      Number of bins used in the pseudo-ordering of gradient
                       modulus.
                       Suggested value: 1024


    @return            A double array of size 7 x n_out, containing the list
                       of line segments detected. The array contains first
                       7 values of line segment number 1, then the 7 values
                       of line segment number 2, and so on, and it finish
                       by the 7 values of line segment number n_out.
                       The seven values are:
                       - x1,y1,x2,y2,width,p,-log10(NFA)
                       .
                       for a line segment from coordinates (x1,y1) to (x2,y2),
                       a width 'width', an angle precision of p in (0,1) given
                       by angle_tolerance/180 degree, and NFA value 'NFA'.
                       If 'out' is the returned pointer, the 7 values of
                       line segment number 'n+1' are obtained with
                       'out[7*n+0]' to 'out[7*n+6]'.
*/