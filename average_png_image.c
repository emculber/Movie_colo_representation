#include <stdlib.h>
#include <stdio.h>
#include <png.h>
#include <math.h>

#include <sys/types.h>
#include <dirent.h>
#include <string.h>

int width, height;
png_byte color_type;
png_byte bit_depth;
png_bytep *row_pointers;

void read_png_file(char *filename) {
  FILE *fp = fopen(filename, "rb");

  png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if(!png) abort();

  png_infop info = png_create_info_struct(png);
  if(!info) abort();

  if(setjmp(png_jmpbuf(png))) abort();

  png_init_io(png, fp);

  png_read_info(png, info);

  width      = png_get_image_width(png, info);
  height     = png_get_image_height(png, info);
  color_type = png_get_color_type(png, info);
  bit_depth  = png_get_bit_depth(png, info);

  // Read any color_type into 8bit depth, RGBA format.
  // See http://www.libpng.org/pub/png/libpng-manual.txt

  if(bit_depth == 16)
    png_set_strip_16(png);

  if(color_type == PNG_COLOR_TYPE_PALETTE)
    png_set_palette_to_rgb(png);

  // PNG_COLOR_TYPE_GRAY_ALPHA is always 8 or 16bit depth.
  if(color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
    png_set_expand_gray_1_2_4_to_8(png);

  if(png_get_valid(png, info, PNG_INFO_tRNS))
    png_set_tRNS_to_alpha(png);

  // These color_type don't have an alpha channel then fill it with 0xff.
  if(color_type == PNG_COLOR_TYPE_RGB ||
     color_type == PNG_COLOR_TYPE_GRAY ||
     color_type == PNG_COLOR_TYPE_PALETTE)
    png_set_filler(png, 0xFF, PNG_FILLER_AFTER);

  if(color_type == PNG_COLOR_TYPE_GRAY ||
     color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
    png_set_gray_to_rgb(png);

  png_read_update_info(png, info);

  row_pointers = (png_bytep*)malloc(sizeof(png_bytep) * height);
  int y;
  for(y = 0; y < height; y++) {
    row_pointers[y] = (png_byte*)malloc(png_get_rowbytes(png,info));
  }

  png_read_image(png, row_pointers);

  fclose(fp);
}

void write_png_file(char *filename) {
  int y;

  FILE *fp = fopen(filename, "wb");
  if(!fp) abort();

  png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (!png) abort();

  png_infop info = png_create_info_struct(png);
  if (!info) abort();

  if (setjmp(png_jmpbuf(png))) abort();

  png_init_io(png, fp);

  // Output is 8bit depth, RGBA format.
  png_set_IHDR(
    png,
    info,
    width, height,
    8,
    PNG_COLOR_TYPE_RGBA,
    PNG_INTERLACE_NONE,
    PNG_COMPRESSION_TYPE_DEFAULT,
    PNG_FILTER_TYPE_DEFAULT
  );
  png_write_info(png, info);

  // To remove the alpha channel for PNG_COLOR_TYPE_RGB format,
  // Use png_set_filler().
  //png_set_filler(png, 0, PNG_FILLER_AFTER);

  png_write_image(png, row_pointers);
  png_write_end(png, NULL);

  for(y = 0; y < height; y++) {
    free(row_pointers[y]);
  }
  free(row_pointers);

  fclose(fp);
}

long avr_red = 0;
long avr_green = 0;
long avr_blue = 0;
void process_png_file() {
  int y;
  int x;
  for(y = 0; y < height; y++) {
    png_bytep row = row_pointers[y];
    for(x = 0; x < width; x++) {
      png_bytep px = &(row[x * 4]);
      avr_red += px[0]*px[0];
      avr_green += px[1]*px[1];
      avr_blue += px[2]*px[2];
      /*
      avr_red += px[0];
      avr_green += px[1];
      avr_blue += px[2];
      */
      // Do something awesome for each pixel here...
      //printf("%4d, %4d = RGBA(%3d, %3d, %3d, %3d)\n", x, y, px[0], px[1], px[2], px[3]);
    }
  }
  avr_red = sqrt(avr_red/(width * height));
  avr_green = sqrt(avr_green/(width * height));
  avr_blue = sqrt(avr_blue/(width * height));
  /*
  avr_red = avr_red/(width * height);
  avr_green = avr_green/(width * height);
  avr_blue = avr_blue/(width * height);
  */
  printf("Red:%ld, Green:%ld, Blue:%ld\n", avr_red, avr_green, avr_blue);
  for(y = 0; y < height; y++) {
    png_bytep row = row_pointers[y];
    for(x = 0; x < width; x++) {
      png_bytep px = &(row[x * 4]);
      px[0] = avr_red;
      px[1] = avr_green;
      px[2] = avr_blue;
      // Do something awesome for each pixel here...
      //printf("%4d, %4d = RGBA(%3d, %3d, %3d, %3d)\n", x, y, px[0], px[1], px[2], px[3]);
    }
  }
}

int main(int argc, char *argv[]) {
  if(argc != 3) abort();

  DIR *dp;
  struct dirent *ep;

  size_t arglen = strlen(argv[1]);
  dp = opendir(argv[1]);
  int i = 0;
  int count = 0;
  if (dp != NULL) {
    DIR *count_dir;
    count_dir = opendir(argv[1]);
    struct dirent *file;
    while ((file = readdir(count_dir))) {
      count++;
    }
    (void) closedir (count_dir);
    while ((ep = readdir(dp))) {
      i++;
      printf("Processing Frame:%d/%d  ::  ", i, count);
      //puts (ep->d_name);
      char *fullpath = malloc(arglen + strlen(ep->d_name) + 2);
      if (fullpath == NULL) { /* deal with error and exit */ return 0;}
      sprintf(fullpath, "%s/%s", argv[1], ep->d_name);
      
      char *s = strstr(fullpath, "png");      // search for string "hassasin" in buff
      if (s == NULL) {
        continue;
      }

      /* use fullpath */
      printf("Full path of Frame:%s  ::  ", fullpath);

      int y;
      for(y = 0; y < height; y++) {
        free(row_pointers[y]);
      }
      free(row_pointers);

      read_png_file(fullpath);
      process_png_file();

      free(fullpath);
    }
    (void) closedir (dp);
    printf("Total Number of frames:%d\n", i);
  }
  else
    perror ("Couldn't open the directory");


  //read_png_file(argv[1]);
  //process_png_file();
  write_png_file(argv[2]);

  return 0;
}
