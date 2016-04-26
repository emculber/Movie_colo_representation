#include <stdio.h>
#include <stdlib.h>
#include <png.h>

int main (int argc , char * argv [] ){
  FILE *file;
  FILE *fptr;
  unsigned char *buffer;
  unsigned long fileLen;

  //Open file
  file = fopen("./sample.png", "rb");
  fptr = fopen("test.png", "wb");
  if (!file) { 
    fprintf(stderr, "Unable to open file %s", "simple.jpg");
    return 0;
  }

  //Get file length
  fseek(file, 0, SEEK_END);
  fileLen=ftell(file);
  fseek(file, 0, SEEK_SET);

  fprintf(stderr, "File Length:%lu\n", fileLen);

  //Allocate memory
  buffer=(unsigned char *)malloc(fileLen);
  if (!buffer) {
    fprintf(stderr, "Memory error!");
    fclose(file);
    return 1;
  }

  fread(buffer,fileLen,sizeof(unsigned char),file);
  fclose(file);

  int i=0;
  while (i < fileLen) {
    //printf("%02X ",((unsigned char)buffer[i]));
    i++;
    if( ! (i % 32) ) {
      //printf( "\n");
    }
  }
  /*
  i = 100;
  while (i < 110) {
      i++;
  }
  */

  fwrite(buffer,1, fileLen,fptr); 

  printf("Buffer Size: %lu\n", sizeof(buffer));
  printf("Buffer 0: %d\n", buffer[0]);
  printf("Buffer 1: %d\n", buffer[1]);

  printf("\nTotal number of pixels: %d\n", i);
  fclose(fptr);

  file = fopen("test.png", "rb");
  fseek(file, 0, SEEK_END);
  fileLen=ftell(file);
  fseek(file, 0, SEEK_SET);
  fprintf(stderr, "File Length:%lu\n", fileLen);

  printf("Buffer Size: %lu\n", sizeof(buffer));
  printf("Buffer: %d\n", buffer[0]);
  printf("Buffer: %d\n", buffer[1]);
  return 0;
}
