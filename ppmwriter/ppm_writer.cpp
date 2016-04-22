#include "ppm_writer.h"

//constructor
PPMwriter::PPMwriter(){}

void PPMwriter::create_image(char* filename, int width, int height){
  ppmFile = std::fopen(filename, "wb");
  if (!ppmFile) {
    std::cout << "Could not open image file" << std::endl;
    exit(1);
  }
  
  WIDTH = width;
  HEIGHT = height;
  imgData = new char[width*height*3];
  
  char header[128];
  std::sprintf(header, "P6\n%5d %5d\n255\n", width, height);
  std::fputs(header, ppmFile);
}


void PPMwriter::write_pixel(char r, char g, char b){
  imgData[curPix++] = r;
  imgData[curPix++] = g;
  imgData[curPix++] = b;
}


void PPMwriter::output_image(){
  std::fwrite((void*)imgData, WIDTH*HEIGHT*3, 1, ppmFile);
//   std::fflush(ppmFile);
  std::fclose(ppmFile);
  
  delete imgData;
}

