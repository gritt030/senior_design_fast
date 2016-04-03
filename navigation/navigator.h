
#ifndef NAVIGATOR_H_
#define NAVIGATOR_H_

#include "navigationmap/navigationmap.h"
#include "./../localization/localizer.h"

class Navigator
{
public:
  static const int WALL_DISTANCE = 100; //cm to get to wall
  static const int ACCURACY = 100;      //cm accuracy when path following
  static const int SONAR_DIFF = 100;    //cm diff between side sonars
  static const int DIST_WEIGHT = 1;     //weight of dist when path planning (lower = more influential)
  static const int OPEN_WEIGHT = 1;     //weight of "openness" when path planning (lower = more influential)
  
  static const int MAX_WEIGHT = 4294967295;
  
  Navigator(NavigationMap* map);
  ~Navigator();
  
  void getNavigationStep(int* currentCoords, int* currentSonars, int* buffer);
  void returnToStart(int* currentCoords);
  
private:
  class Path{
  public:
    int x, y;
    Path* next = nullptr;
  };

private:
  class QueueNode{
  public:
    int x, y, value;
    QueueNode* next = nullptr;
  };
  
private:
  NavigationMap* navMap;
  Path* currentPath = nullptr;
  
  Path* getFrontierPath(int* buffer, int* currentCoords);
  Path* getPathToPoint(int x, int y, int* currentCoords);
  
  void insertInQueue(QueueNode* node, QueueNode* queue);
  Path* generatePathFromGrid(char* nextGrid);
};

#endif // NAVIGATOR_H
