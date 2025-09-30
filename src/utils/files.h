#ifndef __FILES_H__
#define __FILES_H__

#include <stdarg.h>
#include <stdbool.h>
#include "../includes/jansson/jansson.h"


/* Struct for safe handling of dynamically allocated strings from files */
typedef struct {
  char*   data;
  int     size;
} FileString;


/* General file helpers */
bool file_exists(const char* _path);

const char* load_file_as_string(const char* _filename);

FileString create_file_string(const char* _filename);

void destroy_file_string(FileString* fstr);

int write_string_to_file(const char* _str, const char* _filename);


/* Json management */
int json_parse(json_t **json_object, const char* raw_text, int text_len);

json_t* json_load_from_file(const char* _filename);

int json_save_to_file(json_t* _json, const char* _filename);


/* Directory management */
int create_directory_if_not_exists(const char* _path);

bool directory_exists(const char* _path);


#endif
