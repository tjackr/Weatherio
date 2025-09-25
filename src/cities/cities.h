#ifndef __CITIES_H__
#define __CITIES_H__

#include "../meteo/meteo.h"

typedef struct City City;

typedef struct City
{
	City*     prev;
	City*     next;

	char*     name;
	float     lat;
	float     lon;

  Weather*  weather;
  Forecast* forecast;

} City;

typedef struct
{
	City*     head;
	City*     tail;

} Cities;


int cities_init(Cities* c);

int cities_print(Cities* _Cities);

void cities_dispose(Cities* c);


int city_add(Cities* _Cities, char* _Name, float _Latitude, float _Longitude, City** _City);

int city_get_by_index(Cities* _cities, int* _cities_count, int* _index, City** _city);

int city_get_by_name(Cities* _Cities, const char* _Name, City** _CityPtr);

int city_get_temperature(City* _City/* , float* _Temperature */);

void city_remove(Cities* _Cities, City* _City);

#endif 
