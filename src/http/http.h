#ifndef __HTTP_H__
#define __HTTP_H__

#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

typedef struct {
  char* addr;  /* Pointer to address for chunk */
  size_t size; /* Size of chunk */
} HTTP;

int http_init(HTTP* _data);

int curl_get_response(HTTP* _data, const char* _url);

void http_dispose(HTTP* _data);

#endif
