#ifndef __Meteo_h__
#define __Meteo_h__

#include "../includes/jansson/jansson.h"


#define METEO_BASE_URL "https://api.open-meteo.com/v1/forecast?latitude=%f&longitude=%f%s"

extern const char* CACHE_PATH;

typedef struct
{
  double      timestamp;
  int         weather_code; 

  double      temperature; 
  char*       temperature_unit; 

  double      windspeed; 
  char*       windspeed_unit;

  double      winddirection; 
  char*       winddirection_unit;

  double      precipitation;
  char*       precipitation_unit;

} Weather;

typedef struct
{
  int         count;
  Weather*    weather;

} Forecast;


int meteo_get_current_weather(float _lat, float _lon, Weather* weather, json_t** _full_json);


#endif 
