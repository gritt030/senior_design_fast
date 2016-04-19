#ifndef __REGION_DATASTRUCTURE__
#define __REGION_DATASTRUCTURE__
#define THR1 15
#define THR2 0.7
#define THR3 22.5
#define DIST(x1, y1, x2, y2) (x2-x1)*(x2-x1) + (y2-y1)*(y2-y1) 
#define DENSITY(lx,ly,numOfMembers) numOfMembers/float(lx*ly)
#define WIDTH 320
#define HEIGHT 240
#define USED 0
#define NOT_USED 255
#define ALIGNED_THR 8

#include <Eigen/Core>  // To use Eigen::Vector2d
#include "../lsd.h"


using namespace std;


typedef struct {

    Eigen::Vector2d RecPnt1, RecPnt2;
    int cx, cy;
} Rectangle;

class region {

  public:

    int RegWidth, RegHeight;
    int unique_id_;
    Rectangle rectangle;
    float orientation;
    int rect_size_x, rect_size_y;
    int *memberPixelIDs;
    int *boundaryPixelIDs;
    int memberPixelIDsCount, boundaryPixelIDsCount;
    int *y;
    int *x;

    float* atan2_table = NULL;
    const int numberOfAngles = 45;  /// In only the first 45 degree of one quadrant.

    void create_atan2_table() {
      int i;
      atan2_table = (float*) malloc(sizeof(float)*(numberOfAngles+1));

      for (i = 0; i <= numberOfAngles; ++i) {
        atan2_table[i] = (float) atan(static_cast<float>(i)/numberOfAngles);
      }
    }

    void delete_atan2_table() {
      free((void *) atan2_table);
    }

    // Returns on interval [-pi, pi]
    double table_based_atan2(double y, double x) {
      double ang = 0;

      float absx = fabs(x), absy = fabs(y);

      if (absx < absy) {
        ang = atan2_table[(int)(absx/absy * numberOfAngles + 0.5)];
      } else {
        ang = atan2_table[(int)(absy/absx * numberOfAngles + 0.5)];
      }

      if (y >= 0) {
        if (x >= 0) {
          // Quadrant I
          // No changes needed
          if (absx < absy) {
            ang = M_PI*0.5 - ang;
          }
        } else {
          // Quadrant II
          if (absx < absy) {
            ang += M_PI*0.5;
          } else {
            ang = M_PI - ang;
          }
        }
      } else {
        if (x < 0) {
          // Quadrant III
          if (absx > absy) {
            ang -= M_PI;
          } else {
            ang = -ang - M_PI*0.5;
          }
        } else {
          // Quadrant IV
          if (absx > absy) {
            ang = -ang;
          } else {
            ang = -M_PI*0.5+ang;
          }
        }

      }

      return ang;
    }

    void allocate(int size)
    {
        memberPixelIDs = new int[size];
        boundaryPixelIDs = new int[size];
        y = new int[size];
        x = new int[size];
        memberPixelIDsCount = 0;
        boundaryPixelIDsCount = 0;
        unique_id_ = 0;
        create_atan2_table();
    }
    void reset()
    {
        memberPixelIDsCount = 0;
        boundaryPixelIDsCount = 0;
        unique_id_ = 0;
    }
    void deallocate()
    {
        delete_atan2_table();
        delete boundaryPixelIDs;
        delete memberPixelIDs;
        delete x;
        delete y;
    }

    int angle_diff_signed(int a, int b)
    {
        a -= b;
        while( a <= -180 ) a += 360;
        while( a >   180 ) a -= 360;
        return a;
    }

    float computeNewThreshold(int* offset, int *theta)
    {
        /* compute the new mean angle and tolerance */
        int id = memberPixelIDs[0];
        int tempOffset = offset[id];
        int yc = tempOffset / WIDTH;
        int xc = tempOffset - yc * WIDTH;
        int ang_c = theta[id];
        int ang_d, sum_d = 0, s_sum_d = 0;
        float n = 0;

        int  tempX, tempY;
        for(int i = 1; i < memberPixelIDsCount; i++)
        {
          tempOffset = offset[memberPixelIDs[i]];
          tempY = tempOffset / WIDTH;
          tempX = tempOffset - tempY * WIDTH;

          if( DIST( xc, yc, tempX, tempY) < RegWidth * RegWidth )
            {
              ang_d = angle_diff_signed(theta[memberPixelIDs[i]], ang_c);
              sum_d += ang_d;
              s_sum_d += ang_d * ang_d;
              n++;
            }
        }
        float mean_angle = sum_d / n;
        float tau = 2.0 * sqrt( (s_sum_d - 2.0 * mean_angle * sum_d) / n
                             + mean_angle*mean_angle ); /* 2 * standard deviation */
        return tau;
    }

    void calcOrientation(int *G, int *offset, int width)
    {
        float sx = 0, sy = 0, s = 0, mxx = 0, myy = 0, mxy = 0;
        int memberPixelSize = memberPixelIDsCount;
        int tempOffset, tempG, id;

        for(int i = 0; i < memberPixelSize; i++)
        {
            id = memberPixelIDs[i];
            tempOffset = offset[id];
            tempG = G[id];
            y[id] = tempOffset / width;
            x[id] = tempOffset - y[id] * width;
            sy += y[id] * tempG;
            sx += x[id] * tempG;
            s += tempG;
        }

        rectangle.cx = sx / s;
        rectangle.cy = sy / s;

        int tempX, tempY;
        for (int i = 0; i < memberPixelSize; i++)
        {
            id = memberPixelIDs[i];
            tempX = x[id];
            tempY = y[id];
            tempG = G[id];
            mxx += tempG * (tempX - rectangle.cx) * (tempX - rectangle.cx);
            myy += tempG * (tempY - rectangle.cy) * (tempY - rectangle.cy);
            mxy += tempG * (tempX - rectangle.cx) * (tempY - rectangle.cy);
        }

        mxx /= s;
        mxy /= s;
        myy /= s;

        orientation = 0.5 * atan2(-2*mxy, -myy + mxx);

        if(orientation > 0.0)
            orientation += 3.14159265;

        orientation = -orientation;
    }

    void calcRectangle()
    {

        int boundarySize = boundaryPixelIDsCount;
        float minDx, minDy, maxDx, maxDy, tempDx, tempDy;
        int x1, y1, x2, y2;
        float cosOrientation = cos(orientation);
        float sinOrientation = sin(orientation);
        int tempX, tempY;

        //-------find max of dx and dy and min of dx and dy---------//
        int tempBoundaryPixelID = boundaryPixelIDs[0];

        tempY = y[tempBoundaryPixelID];
        tempX = x[tempBoundaryPixelID];

        minDx = maxDx = cosOrientation * tempX + sinOrientation * tempY;
        minDy = maxDy = sinOrientation * tempX - cosOrientation * tempY;

        for(int i = 1; i < boundarySize; i++)
        {
            tempBoundaryPixelID = boundaryPixelIDs[i];
            tempY = y[tempBoundaryPixelID];
            tempX = x[tempBoundaryPixelID];

            tempDx = cosOrientation * tempX + sinOrientation * tempY;
            tempDy = sinOrientation * tempX - cosOrientation * tempY;

            if( tempDx > maxDx)
                maxDx = tempDx;
            else if( tempDx < minDx )
                minDx = tempDx;

            if( tempDy > maxDy)
                maxDy = tempDy;
            else if( tempDy < minDy )
                minDy = tempDy;
        }

        rect_size_x = abs(maxDx - minDx);
        rect_size_y = abs(maxDy - minDy);
        if(rect_size_x > rect_size_y)
        {
            RegWidth = rect_size_y;
            RegHeight = rect_size_x;
        }else
        {
            RegWidth = rect_size_x;
            RegHeight = rect_size_y;
        }

        int centerDx = cosOrientation * rectangle.cx + sinOrientation * rectangle.cy;        

        x1 = rectangle.cx + (-centerDx + minDx)*cosOrientation;
        y1 = rectangle.cy + (-centerDx + minDx)*sinOrientation;
        x2 = rectangle.cx + (-centerDx + maxDx)*cosOrientation;
        y2 = rectangle.cy + (-centerDx + maxDx)*sinOrientation;            


        x1 = (x1<<1) + 1;
        y1 = (y1<<1) + 1;
        x2 = (x2<<1) + 1;
        y2 = (y2<<1) + 1;

        rectangle.RecPnt1 << x1, y1;
        rectangle.RecPnt2 << x2, y2;
    }

    void regionGrow(int seedPixelID, dataStructure &sobel)
    {
        float theta_region, cosTheta_region, sinTheta_region;
        int id;
        memberPixelIDsCount = 0;
        boundaryPixelIDsCount = 0;
        memberPixelIDs[memberPixelIDsCount] = seedPixelID;
        memberPixelIDsCount++;
        theta_region = sobel.theta[seedPixelID];
        cosTheta_region = sobel.cosTheta[seedPixelID];
        sinTheta_region = sobel.sinTheta[seedPixelID];
        for(int regionPix = 0; regionPix < memberPixelIDsCount; regionPix++ )
        {
            int numOfNeighborAdded = 0;
            int aligned = 0;
            for(int neighbor = 0; neighbor < sobel.neighbor_size[memberPixelIDs[regionPix]]; neighbor++)
            {
                id = sobel.neighbor[memberPixelIDs[regionPix]][neighbor];
                if(fabs(sobel.theta[id] - theta_region) < THR3)
                {
                    numOfNeighborAdded++;
                    if(sobel.status[id] == NOT_USED)
                    {
                        aligned++;
                        sinTheta_region += sobel.sinTheta[id];
                        cosTheta_region += sobel.cosTheta[id];
                        if(aligned == ALIGNED_THR){
                            //theta_region = atan_phi3[fast_atan3(sinTheta_region, cosTheta_region)];
                            theta_region = table_based_atan2(sinTheta_region, cosTheta_region)*57.2958;
                            aligned = 0;
                        }
                        
                        sobel.status[id] = USED;
                        memberPixelIDs[memberPixelIDsCount] = (id);
                        memberPixelIDsCount++;
                    }
                }
            }
            if(numOfNeighborAdded < 6 && numOfNeighborAdded > 0) //is in the boundaries
            {
                boundaryPixelIDs[boundaryPixelIDsCount] = memberPixelIDs[regionPix];
                boundaryPixelIDsCount++;
            }
        }
    }
    // refinement by calculating a new threshold
    void refine(dataStructure &sobel) {
        float theta_region, cosTheta_region, sinTheta_region;
        int id;
        //recover status of pixels
        float newThrh = computeNewThreshold(sobel.offset, sobel.theta);
        for(int i = 0; i < memberPixelIDsCount; i++)
            sobel.status[memberPixelIDs[i]] = NOT_USED;
        int temp_seedPixelID = memberPixelIDs[0]; 
        memberPixelIDsCount = 0;
        boundaryPixelIDsCount = 0;
        memberPixelIDs[memberPixelIDsCount] = temp_seedPixelID;//not needed
        memberPixelIDsCount++;
        theta_region = sobel.theta[temp_seedPixelID];
        cosTheta_region = sobel.cosTheta[temp_seedPixelID];
        sinTheta_region = sobel.sinTheta[temp_seedPixelID];
        int aligned = 0;
        for(int regionPix = 0; regionPix < memberPixelIDsCount; regionPix++ )
        {
            int numOfNeighborAdded = 0;
            for(int neighbor = 0; neighbor < sobel.neighbor_size[memberPixelIDs[regionPix]]; neighbor++)
            {
                id = sobel.neighbor[memberPixelIDs[regionPix]][neighbor];
                if(fabs(sobel.theta[id] - theta_region) < newThrh)
                {
                    numOfNeighborAdded++;
                    if(sobel.status[id] == NOT_USED)
                    {
                        aligned++;
                        sinTheta_region += sobel.sinTheta[id];
                        cosTheta_region += sobel.cosTheta[id];
                        if(aligned == ALIGNED_THR){
                            //theta_region = atan_phi3[fast_atan3(sinTheta_region, cosTheta_region)];                                
                            theta_region = table_based_atan2(sinTheta_region, cosTheta_region)*57.2958;
                            aligned = 0;
                        }
                        sobel.status[id] = USED;
                        memberPixelIDs[memberPixelIDsCount] = (id);
                        memberPixelIDsCount++;
                    }
                }
            }
            if(numOfNeighborAdded < 6 && numOfNeighborAdded > 0) //is in the boundaries
            {
                boundaryPixelIDs[boundaryPixelIDsCount] = memberPixelIDs[regionPix];
                boundaryPixelIDsCount++;
            }
        }
    }
    // refinement by cutting part of rectangle 
    void rectCut(dataStructure &sobel) {
        int tempOffset = sobel.offset[memberPixelIDs[0]];
        int yc = tempOffset / WIDTH;
        int xc = tempOffset - yc * WIDTH;
        float rad1 = DIST( xc, yc, rectangle.RecPnt1(0), rectangle.RecPnt1(1) );
        float rad2 = DIST( xc, yc, rectangle.RecPnt2(0), rectangle.RecPnt2(1) );
        float rad = rad1 > rad2 ? rad1 : rad2;
        int x, y;
        int tempMemberPixelIDSsize = memberPixelIDsCount;
        int tempBoundaryPixelIDSsize = boundaryPixelIDsCount;
        float density = 0;
        while( (density < THR2) && (memberPixelIDsCount > THR1) )
        {
            rad *= 0.5625;
            for(int i = 0; i < tempMemberPixelIDSsize; i++)
            {
                tempOffset = sobel.offset[memberPixelIDs[i]];
                y = tempOffset / WIDTH;
                x = tempOffset - y * WIDTH;
                if( DIST( xc, yc, x, y ) > rad )
                {
                    sobel.status[memberPixelIDs[i]] = NOT_USED;
                    memberPixelIDs[i] = memberPixelIDs[tempMemberPixelIDSsize-1]; // if i==*reg_size-1 copy itself
                    memberPixelIDsCount--;
                    tempMemberPixelIDSsize--;
                    --i; //to avoid skipping one point
                }
            }
            for(int i = 0; i < tempBoundaryPixelIDSsize; i++)
            {
                tempOffset = sobel.offset[boundaryPixelIDs[i]];
                y = tempOffset / WIDTH;
                x = tempOffset - y * WIDTH;
                if( DIST( xc, yc, x, y ) > rad )
                {
                    boundaryPixelIDs[i] = boundaryPixelIDs[tempBoundaryPixelIDSsize-1]; // if i==*reg_size-1 copy itself
                    boundaryPixelIDsCount--;
                    tempBoundaryPixelIDSsize--;
                    --i; //to avoid skipping one point
                }
            }
            this->calcOrientation(sobel.G, sobel.offset, WIDTH);
            this->calcRectangle();
            density = DENSITY(rect_size_x, rect_size_y, memberPixelIDsCount);
        }
    }
};

#endif