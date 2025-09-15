#ifndef CITIES_H
#define CITIES_H

#ifndef CITYURL_H
#define CITYURL_H

typedef struct 
{
  char    addr[64];
  float   lat;
  float   lon;

} city;

typedef struct
{
  char    name[32];
  float   temp;
  char    unit[10];

} weather;

const char* cities();

void cityurl(const char* cities, char* url);

#endif
#endif
