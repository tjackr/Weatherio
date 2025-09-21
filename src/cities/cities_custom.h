#ifndef __CITIES_H__
#define __CITIES_H__

/* #include "../weather/weather.h" */

typedef struct 
{
  int     id;
  char*   name;
  float   lat;
  float   lon;

  /* weather current_weather; */

} city;

void citystring_parse(const char* citystring, city *cities_ptr);

void build_meteourl (char* url, city city);

#endif
