#ifndef __WEATHER_H__
#define __WEATHER_H__

#include <time.h>


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


/*============== Public functions ==============*/

int weather_get(float _lat, float _lon, Weather* _Weather, Forecast* _Forecast, bool _hourly);

void weather_dispose(Weather** _Weather_Ptr, Forecast** _Forecast_Ptr);

/*==============================================*/


#endif
