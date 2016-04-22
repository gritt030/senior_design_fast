#ifndef PPM_WRITER_H
#define PPM_WRITER_H

#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <string.h>
#include <cmath>

class PPMwriter
{
public:
  void create_image(char* filename, int width, int height);
  void write_pixel(char r, char g, char b);
  void output_image();
  
  int WIDTH, HEIGHT;
  
  //constructor
  PPMwriter();

private:
  char* imgData;
  int curPix = 0;
  FILE *ppmFile;
  std::ofstream* ppmStream;
};

#endif // PNG_WRITER_H
