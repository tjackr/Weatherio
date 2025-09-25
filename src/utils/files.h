#ifndef __FILES_H__
#define __FILES_H__

#include <stdarg.h>
#include "../includes/cJSON.h"

/* Struct for safe handling of dynamically allocated strings from files */
typedef struct {
  char*   data;
  int     size;
} FileString;

const char* load_file_as_string(const char* _filename);

FileString create_file_string(const char* _filename);

void destroy_file_string(FileString* fstr);

int write_string_to_file(const char* _str, const char* _filename);

int json_parse(cJSON** _cjson_object, const char* _raw_text, int _text_len);

#endif
