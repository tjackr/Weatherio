#include <stdlib.h>

#ifndef HTTP_H
#define HTTP_H

struct memory_chunk {
  char *addr;  /* Pointer to adress for chunk */
  size_t size; /* Size of chunk */
};

char* get_meteo_response(char* url, char* response);

#endif
