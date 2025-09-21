#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "cities_custom.h"


/* Custom loop for tokenizing citystring and building the struct */
void citystring_parse(const char* citystring, city* cities_ptr) {
  const char* ptr       = citystring; /* Like a cursor, moving from char to char */
  const char* tok_start = citystring; 

  char token[64]; /* Temp buffer */
  int part = 0, city_index = 0; /* We expect 3 parts per city index */

  while (*ptr != '\0')
  {
    if (*ptr == ':' || *ptr == '\n')
    {
      size_t length = ptr - tok_start;
      memcpy(token, tok_start, length); /* Copy token data to temp buffer */
      token[length] = '\0'; /* null terminate last byte to make safe */
      printf("city-index: %i - ", city_index);
      printf("token: %s\n", token);
      
      /* Copy tokens to struct */
      if (part == 0)
      {
        cities_ptr[city_index].name = (char*)malloc(sizeof(char) * length); /* Allocate memory for the name */
        strcpy(cities_ptr[city_index].name, token);
      }
      else if (part == 1)
        cities_ptr[city_index].lat = atof(token);
      else if (part == 2)
        cities_ptr[city_index].lon = atof(token);

      part = (part + 1) % 3; /* When part is dividable by 3 we have a new index */ 
      
      if (part == 0) {
        cities_ptr[city_index].id = (city_index + 1);
        city_index++;
      }

      /* Set token start to the upcoming token iteration */
      tok_start = ptr + 1; 
    }
    ptr++;
  }
}

