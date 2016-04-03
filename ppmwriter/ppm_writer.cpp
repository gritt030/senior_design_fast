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
  
  char header[128];
  std::sprintf(header, "P6\n%5d %5d\n255\n", width, height);
  std::fputs(header, ppmFile);
}


void PPMwriter::write_pixel(char r, char g, char b){
  std::fwrite((void*)&r, 1,1, ppmFile);
  std::fwrite((void*)&g, 1,1, ppmFile);
  std::fwrite((void*)&b, 1,1, ppmFile);
}


void PPMwriter::output_image(){
  std::fflush(ppmFile);
  std::fclose(ppmFile);
}

