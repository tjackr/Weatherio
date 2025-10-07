#ifndef __Meteo_h__
#define __Meteo_h__

#include <time.h>
#include "../includes/jansson/jansson.h"


#define METEO_BASE_URL "https://api.open-meteo.com/v1/forecast?latitude=%f&longitude=%f%s"

/*============== Global variables ==============*/

extern const char* CACHE_PATH;

/*==============================================*/

typedef struct
{
  time_t      timestamp;
  int         weather_code; 

  double      temperature; 
  const char* temperature_unit; 

  double      windspeed; 
  const char* windspeed_unit;

  int         winddirection; 
  const char* winddirection_unit;

  double      precipitation;
  const char* precipitation_unit;

} Weather;

typedef struct
{
  int         count;
  Weather*    weather;

} Forecast;


int meteo_get_weather(float _lat, float _lon, Weather* _Weather, json_t** _full_json, bool _forecast);

int meteo_get_forecast(float _lat, float _lon, Forecast* _Forecast, json_t** _full_json);


#endif 
