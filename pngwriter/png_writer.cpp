
#include "png_writer.h"

//constructor
PngWriter::PngWriter(){
}

void PngWriter::create_image(char* filename, int width, int height){
  this->filename = (char*) malloc(101); //TODO: possible buffer overflow here
  strcpy(this->filename, filename);
  this->width = width;
  this->height = height;
  
  row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * height);
  int rowbytes = width * 4;
  
  for (int y=0; y<height; y++){
    row_pointers[y] = (png_byte*) malloc(rowbytes);
  }
  
  initialize_color(0x000000ff);
  
  //number_of_passes = png_set_interlace_handling(png_ptr);
  color_type = PNG_COLOR_TYPE_RGB_ALPHA;//png_get_color_type(png_ptr, info_ptr);
  bit_depth = 8;//png_get_bit_depth(png_ptr, info_ptr);
}


void PngWriter::initialize_color(int color){
  char a = (char)(color & 0x000000ff);
  char b = (char)((color & 0x0000ff00) >> 8);
  char g = (char)((color & 0x00ff0000) >> 16);
  char r = (char)((color & 0xff000000) >> 24);
  
  for (int y=0; y<height; y++){
    png_byte* row = row_pointers[y];
    for (int x=0; x<width; x++){
      png_byte* ptr = &row[4*x];
      ptr[0] = r;
      ptr[1] = g;
      ptr[2] = b;
      ptr[3] = a;
    }
  }
}


void PngWriter::set_pixel(int x, int y, int color){
  if ((x > width) || (y > height) || (x < 0) || (y < 0)){
    std::cout << "Pixel coordinates out of bounds" << std::endl;
    return;
  }
  
  png_byte* row = row_pointers[y];
  png_byte* ptr = &row[4*x];
  
  ptr[3] = (char)(color & 0x000000ff);
  ptr[2] = (char)((color & 0x0000ff00) >> 8);
  ptr[1] = (char)((color & 0x00ff0000) >> 16);
  ptr[0] = (char)((color & 0xff000000) >> 24);
}


void PngWriter::output_image(){
  //std::cout << "Writing image: " << this->filename << std::endl;
  
  this->open_file();
  //std::cout << "    Initializing file" << std::endl;
  this->initialize_file();
  //std::cout << "    Writing header" << std::endl;
  this->write_header();
  //std::cout << "    Writing image data" << std::endl;
  this->write_image_data();
  //std::cout << "    Finalizing file" << std::endl;
  this->finalize_write();
  //std::cout << "    Cleaning up" << std::endl;
  this->cleanup();
  //std::cout << "Done writing image" << std::endl;
}

void PngWriter::open_file(){
  pngFile = std::fopen(filename, "wb");
    if (!pngFile) {
      std::cout << "Could not open file" << std::endl;
      exit(1);
    }
}

void PngWriter::initialize_file(){ 
  png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

  if (!png_ptr){
    std::cout << "Could not create png ptr" << std::endl;
    exit(1);
  }

  info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr){
    std::cout << "Could not create info struct" << std::endl;
    exit(1);
  }

  if (setjmp(png_jmpbuf(png_ptr))){
    std::cout << "Could not jump?" << std::endl;
    exit(1);
  }

  png_init_io(png_ptr, pngFile);
}

void PngWriter::write_header(){
  if (setjmp(png_jmpbuf(png_ptr))){
    std::cout << "Could not write header" << std::endl;
    exit(1);
  }

  png_set_IHDR(png_ptr, info_ptr, width, height,
                bit_depth, color_type, PNG_INTERLACE_NONE,
                PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

  png_write_info(png_ptr, info_ptr);
}

void PngWriter::write_image_data(){
  if (setjmp(png_jmpbuf(png_ptr))){
    std::cout << "Error while writing bytes" << std::endl;
    exit(1);
  }

  png_write_image(png_ptr, row_pointers);
}

void PngWriter::finalize_write(){
  if (setjmp(png_jmpbuf(png_ptr))){
    std::cout << "Could not end write" << std::endl;
    exit(1);
  }

  png_write_end(png_ptr, NULL);
  std::fflush(pngFile);
  std::fclose(pngFile);
}

void PngWriter::cleanup(){
  for (int y=0; y<height; y++) {
    free(row_pointers[y]);
  }
  free(row_pointers);
}