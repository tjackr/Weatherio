#ifndef __Meteo_h__
#define __Meteo_h__

#define METEO_BASE_URL "https://api.open-meteo.com/v1/forecast?latitude=%f&longitude=%f%s"

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

/*
typedef struct
{
  const char* api_url;
  char*       timezone;

} Meteo;
*/

int meteo_get_current_weather(float _lat, float _lon, Weather* weather);

#endif 
