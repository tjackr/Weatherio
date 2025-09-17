#ifndef CITIES_H
#define CITIES_H

typedef struct 
{
  char    name[64];
  float   lat;
  float   lon;

} city;

/* const char* cities(); */

/* void cityurl(const char* cities, char* url); */

void citystring_parse(const char* citystring, city* cities_ptr);

void build_meteourl (char* url, city city);

#endif
