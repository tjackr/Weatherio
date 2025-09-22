#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "files.h"

#include "../includes/cJSON.h"

/* Read file contents into a char array using fopen, fseek, fread and manual memory allocation 
 * Resources: 
 * https://cplusplus.com/reference/cstdio/fread
 * */
const char* load_file_as_string(const char* _filename) {
    
  FILE* file = fopen(_filename, "r");
  if (!file) { printf("File load error: %s\n", _filename); exit (0); }
  
  fseek(file, 0, SEEK_END); /* Seek end of file */
  int file_size = ftell(file); /* Define filesize based on fseek position */
  rewind(file); /* rewind to beginning of file */

  char* buffer = (char*)malloc(sizeof(char)*file_size);
  if (buffer == NULL) { printf("Malloc error"); exit (1); }
  
  int result = fread(buffer, 1, file_size, file); /* reads file into buffer and returns the amount of bytes read */
  if (result != file_size) { printf("File read error"); exit (2); }

  fclose(file);
  return buffer;
}


/* This isn't tested yet, but should be useful for when we want to save new city to our citylist */
int save_string_to_file (const char* _str, char* _filename)
{
  FILE* file = fopen (_filename, "w");
  if (!file)
  {
    printf("Error opening file: %s", (char*)file);
    return -1;
  }
  fputs (_str, file);
  fclose (file);

  return 0;
}

/*Writes json string to file*/
int json_handler (char* _str, char* _filename, char* _cityname)
{
  printf("JSON Handler called\n");
   /*write the JSON string to a file*/
   FILE *fp = fopen(_filename, "w");
   if (fp == NULL) {
       printf("Error: Unable to open the file.\n");
       return 1;
   }

   printf("%s\n", _filename);
   fputs(_str, fp);
   fclose(fp);


   return 0;
}