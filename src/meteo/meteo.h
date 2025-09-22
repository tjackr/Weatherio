#ifndef __Meteo_h__
#define __Meteo_h__

#include <stdbool.h>

typedef struct
{
  float       temp;
  int         type; /* Cloudy/Sunny etc. */
  float       windspeed;

} Weather;

typedef struct
{
  int         datetime;
  Weather*    weather;

} Forecast;


typedef struct
{
  const char* api_url;
  Weather*    current_weather;
  Forecast*   forecast;

  /* API settings here
   * Open Meteo API reference:
   * https://open-meteo.com/en/docs#api_documentation
   * */
  bool        current; /* Get current data only */
  bool        celsius;
  char*       timezone;

} Meteo;

int meteo_init(Meteo* _meteo, const char* _api_url);

int meteo_get_temperature(Meteo* _meteo, float _lat, float _lon, char* _name/* , float* _temperature */);

void meteo_dispose(Meteo* _meteo);

#endif 
