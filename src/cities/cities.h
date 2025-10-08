#ifndef __CITIES_H__
#define __CITIES_H__

#include <stdbool.h>
#include "../meteo/meteo.h"


typedef struct City
{
  struct City* prev;
  struct City* next;

  char* name;
  float lat;
  float lon;

  Weather*  weather;
  Forecast* forecast;

} City;


typedef struct
{
	City*     head;
	City*     tail;

} Cities;


int cities_init(Cities* _Cities);

int cities_print(Cities* _Cities);

void cities_dispose(Cities* _Cities);


int city_add(Cities* _Cities, char* _name, float _latitude, float _longitude, City** _City_Ptr);

int city_save_to_file(const char* _city_name, const char* _hashed_city_name, float _lat, float _lon);

int city_get_by_index(Cities* _Cities, int* _cities_count, int* _index, City** _City_Ptr);

int city_get_by_name(Cities* _Cities, const char* _Name, City** _City_Ptr);

int city_get_weather(City* _City, bool _hourly);

void city_remove(Cities* _Cities, City* _City);


#endif 
