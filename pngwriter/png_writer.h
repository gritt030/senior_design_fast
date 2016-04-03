
#ifndef PNG_WRITER_H_
#define PNG_WRITER_H_

#include <iostream>
#include <cstdlib>

#include <string.h>
#include <png.h>

class PngWriter
{
  public:
    void create_image(char* filename, int width, int height);
    void set_pixel(int x, int y, int color);
    void output_image();
    
    //constructor
    PngWriter();
  
  private:
    int width, height;
    
    png_byte color_type;
    png_byte bit_depth;
    png_structp png_ptr;
    png_infop info_ptr;
    //int number_of_passes;
    
    png_bytep * row_pointers;
    
    FILE *pngFile;
    char* filename;
    
    void open_file();
    void initialize_file();
    void write_header();
    void write_image_data();
    void finalize_write();
    void cleanup();
    void initialize_color(int color);
};

#endif // PNG_WRITER_H
