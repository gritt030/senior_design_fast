
#include "navigator.h"

//constructor
Navigator::Navigator(NavigationMap* map){
  this->navMap = map;
}

//destructor
Navigator::~Navigator(){
  Path* p = this->currentPath;
  Path* next;
  
  while (p != nullptr) {
    next = p->next;
    delete p;
    p = next;
  }
  
  this->currentPath = nullptr;
}


void Navigator::getNavigationStep(int* currentCoords, int* currentSonars, int* buffer){}


void Navigator::returnToStart(int* currentCoords){}


Navigator::Path* Navigator::getFrontierPath(int* buffer, int* currentCoords){
  this->navMap->cleanFrontier();
  
  //grids for path planning algorithm
  int size = OpenGrid::GRID_SIZE*OpenGrid::GRID_SIZE;
  unsigned int* weightMap = new unsigned int[size];
  char* nextMap = new char[size]();
  
  //set all weights to "infinity"
  for (int i=0; i<size; i++){
    weightMap[i] = MAX_WEIGHT;
  }
  
  QueueNode* curNode = new QueueNode();
  curNode->x = currentCoords[0];
  curNode->y = currentCoords[1];
  curNode->value = 0;
  QueueNode* queue = curNode;
  
  int x = currentCoords[0] / Localizer::SCALE;
  int y = currentCoords[1] / Localizer::SCALE;
  int index = x*OpenGrid::GRID_SIZE + y;
  
  nextMap[index] = (char)(-1);
  
  while (queue != nullptr) {
    //pop node from queue
    curNode = queue;
    queue = curNode->next;
    
    x = curNode->x;
    y = curNode->y;
    
    if (navMap->frontiers->getValue(x,y) > navMap->THRESHOLD) {
      return generatePathFromGrid(nextMap);
    }
    
    //explore node to right
    //node has not been explored
    if (nextMap[index + OpenGrid::GRID_SIZE] == 0) {
      int dist = weightMap[index];
      dist += DIST_WEIGHT;
      dist += 0;//FINISH THIS JUNK
    }
  }
  
  return nullptr;
}


Navigator::Path* Navigator::getPathToPoint(int x, int y, int* currentCoords){}




void Navigator::insertInQueue(QueueNode* node, QueueNode* queue){}
Navigator::Path* Navigator::generatePathFromGrid(char* nextGrid){}
