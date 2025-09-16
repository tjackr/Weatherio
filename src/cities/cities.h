#ifndef CITIES_H
#define CITIES_H

typedef struct 
{
  char    name[64];
  char    lat[16];
  char    lon[16];

} city;

const char* cities();

void cityurl(const char* cities, char* url);


#endif
