#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "cities.h"


/* Custom loop for tokenizing citystring and building the struct */
void citystring_parse(const char* citystring, city *cities_ptr) {
  const char *ptr       = citystring; /* Like a cursor, moving from char to char */
  const char *tok_start = citystring; 

  char token[64]; /* Temp buffer, max chars as max chars cityname */
  int part = 0, city_index = 0;

  while (*ptr != '\0')
  {
    if (*ptr == ':' || *ptr == '\n')
    {
      size_t length = ptr - tok_start;
      memcpy(token, tok_start, length); /* Copy token data to temp buffer */
      token[length] = '\0'; /* null terminate last byte to make safe */
      
      /* Copy tokens to struct */
      if (part == 0)
        strcpy(cities_ptr[city_index].name, token);
      else if (part == 1)
        cities_ptr[city_index].lat = atof(token);
      else if (part == 2)
        cities_ptr[city_index].lon = atof(token);

      part = (part + 1) % 3; /* */
      if (part == 0)
        city_index++;

      /* Set token start to the next token */
      tok_start = ptr + 1; 
    }
    ptr++;
  }
}
