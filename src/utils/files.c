#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/stat.h>

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
int json_parse(cJSON** _cjson_object, const char* _raw_text, int _text_len)
{
  if (_raw_text == NULL)
  {
    printf("No text to parse as JSON!\n");
    return -1;
  }

  *_cjson_object = cJSON_ParseWithLength(_raw_text, _text_len);
  if (_cjson_object == NULL) 
  {
    const char *error_ptr = cJSON_GetErrorPtr();
    if (error_ptr != NULL) {
        printf("Error parsing filestring: %s\n", error_ptr);
    }
    cJSON_Delete(*_cjson_object);
    return -2;
  }

  return 0;
}
