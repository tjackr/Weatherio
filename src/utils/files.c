#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <errno.h>

#include "files.h"
#include "../includes/tinydir.h"

/* Returns true or false whether file exists */
bool file_exists(const char* _path) {
  struct stat buffer;
  if (stat(_path, &buffer) == 0) {
    return S_ISREG(buffer.st_mode);
  }
  return false;
}

/* Returns true or false whether directory exists */
bool directory_exists(const char* _path) {
  struct stat buffer;
  if (stat(_path, &buffer) == 0) {
    return S_ISDIR(buffer.st_mode);
  }
  return false;
}

/* Tries to create directory if it doesn't already exist */
int create_directory_if_not_exists(const char* _path) {
  if (directory_exists(_path)) {
    return 0;
  }
  #if defined _WIN32
  bool success = CreateDirectory(_Path, NULL);
  if(success == false)
  {
    DWORD err = GetLastError();
    if(err == ERROR_ALREADY_EXISTS)
      return 1;
    else
      return -1;
  }
	#else
  if (mkdir(_path, 0755) == -1) {
    if (errno != EEXIST) {
      printf("Failed to create directory '%s': %s\n", _path, strerror(errno));
      return -1;
    }
  }
  #endif
  return 0;
}

/* Loads content of file into custom String struct (on heap)
 * IMPORTANT: Needs to manually be freed using destroy_file_string */
FileString create_file_string(const char* _filename) {

  FILE* file = fopen(_filename, "r");
  if (!file) { printf("File load error: %s\n", _filename); exit (0); }
  
  fseek(file, 0, SEEK_END); /* Seek end of file */
  int file_size = ftell(file); /* Define filesize based on fseek position */
  rewind(file); /* Rewind to beginning of file */

  /* Init struct */
  FileString Fstr = {
    (char*)malloc(sizeof(char) * (file_size + 1)), /* Plus one for \0 */
    file_size
  }; 
  if (Fstr.data == NULL) { printf("Malloc error"); exit (1); }

  /* Reads file into buffer and returns the amount of bytes read */
  int read_size = fread(Fstr.data, 1, file_size, file); 
  if (read_size != file_size) { printf("File read error"); exit (2); }

  Fstr.data[file_size] = '\0';

  fclose(file); /* Done with file, close it */

  return Fstr;
}

/* Helper function for clearing memory from custom String */
void destroy_file_string(FileString* _Fstr)
{
  if (_Fstr->size > 0)
  {
    free(_Fstr->data);
    _Fstr->data = NULL;
    _Fstr->size = 0;
  } 
  else { printf("String is empty!"); }
}

/* Writes string to given file */
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

/* Parses jansson object from given string */
int json_parse(json_t** _json_obj_ptr, const char* _raw_text, int _text_len)
{
  if (_raw_text == NULL) {
    printf("No text to parse as JSON!\n");
    return -1;
  }

  json_error_t error;
  *_json_obj_ptr = json_loadb(_raw_text, _text_len, 0, &error);

  if (*_json_obj_ptr == NULL) {
    printf("Error parsing JSON at line %d: %s\n", error.line, error.text);
    return -2;
  }
  return 0;  
}

/* Loads jansson object from given file */
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

/* Writes jansson object to given file */
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
