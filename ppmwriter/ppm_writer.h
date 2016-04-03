#ifndef PPM_WRITER_H
#define PPM_WRITER_H

#include <iostream>
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
  
  //constructor
  PPMwriter();

private:
  int WIDTH;
  FILE *ppmFile;
};

#endif // PNG_WRITER_H
