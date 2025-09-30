#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <ctype.h>
#include <errno.h>

#include "files.h"

bool file_exists(const char *path) {
    struct stat buffer;
    if (stat(path, &buffer) == 0) {
        return S_ISREG(buffer.st_mode);
    }
    return false;
}

/* Read file contents into a char array using fopen, fseek, fread and manual memory allocation 
 * Resources: 
 * https://cplusplus.com/reference/cstdio/fread
 * 
 * WE SHOULD NOT USE THIS, IMPOSSIBLE TO FREE MEMORY
 * */
const char* load_file_as_string(const char* _filename) {
    
  FILE* file = fopen(_filename, "r");
  if (!file) { printf("File load error: %s\n", _filename); exit (0); }
  
  fseek(file, 0, SEEK_END); /* Seek end of file */
  int file_size = ftell(file); /* Define filesize based on fseek position */
  rewind(file); /* rewind to beginning of file */

  char* buffer = (char*)malloc(sizeof(char) * file_size);
  if (buffer == NULL) { printf("Malloc error"); exit (1); }
  
  int result = fread(buffer, 1, file_size, file); /* reads file into buffer and returns the amount of bytes read */
  if (result != file_size) { printf("File read error"); exit (2); }

  fclose(file);
  return buffer;
}

/* Loads content of file into custom String struct (on heap)
 * IMPORTANT: Needs to manually be freed using next function
 * */
FileString create_file_string(const char* _filename) {

  FILE* file = fopen(_filename, "r");
  if (!file) { printf("File load error: %s\n", _filename); exit (0); }
  
  fseek(file, 0, SEEK_END); /* Seek end of file */
  int file_size = ftell(file); /* Define filesize based on fseek position */
  rewind(file); /* Rewind to beginning of file */

  FileString fstr = {
    (char*)malloc(sizeof(char) * (file_size + 1)), /* Plus one for \0 */
    file_size
  }; /* Init struct */

  if (fstr.data == NULL) { printf("Malloc error"); exit (1); }

  /* Reads file into buffer and returns the amount of bytes read */
  int read_size = fread(fstr.data, 1, file_size, file); if (read_size != file_size) { printf("File read error"); exit (2); }

  fstr.data[file_size] = '\0';

  fclose(file); /* Done with file, close it */

  return fstr;
}

/* Helper function for clearing memory from custom String */
void destroy_file_string(FileString* fstr)
{
  if (fstr->size > 0)
  {
    free(fstr->data);
    fstr->data = NULL;
    fstr->size = 0;
  } 
  else { printf("String is empty!"); }
}


/* Writes string to file */
int write_string_to_file(const char* _str, const char* _filename)
{
   FILE *f = fopen(_filename, "w");
   if (f == NULL) {
     printf("Error: Unable to open the file.\n");
     return -1;
   }

   fputs(_str, f);
   fclose(f);

   return 0;
}

/*json parser*/
int json_parse(json_t **json_object, const char* raw_text, int text_len)
{
  if (raw_text == NULL) {
    printf("No text to parse as JSON!\n");
    return -1;
  }

  json_error_t error;
  *json_object = json_loadb(raw_text, text_len, 0, &error);

  if (*json_object == NULL) {
    printf("Error parsing JSON at line %d: %s\n", error.line, error.text);
    return -2;
  }
  return 0;  
}
bool directory_exists(const char *path) {
  struct stat buffer;
  if (stat(path, &buffer) == 0) {
    return S_ISDIR(buffer.st_mode);
  }
  return false;
}

int create_directory_if_not_exists(const char* _path) {
  if (directory_exists(_path)) {
    return 0;
  }
  
  if (mkdir(_path, 0755) == -1) {
    if (errno != EEXIST) {
      printf("Failed to create directory '%s': %s\n", _path, strerror(errno));
      return -1;
    }
  }
  
  return 0;
}

json_t* json_load_from_file(const char* _filename)
{
  json_error_t error;
  json_t* json = json_load_file(_filename, 0, &error);
  
  if (json == NULL) {
    printf("Error loading JSON from file '%s' at line %d, column %d: %s\n", 
       _filename, error.line, error.column, error.text);
    return NULL;
  }
  
  return json;
}

int json_save_to_file(json_t* _json, const char* _filename)
{
  if (_json == NULL) {
    printf("JSON object is NULL\n");
    return -1;
  }
  
  if (json_dump_file(_json, _filename, JSON_INDENT(2) | JSON_PRESERVE_ORDER) != 0) {
    printf("Error saving JSON to file '%s'\n", _filename);
    return -2;
  }
  
  return 0;
}
