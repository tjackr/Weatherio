#ifndef __METEO_H__
#define __METEO_H__

#include "../libs/includes/jansson/jansson.h"


#define METEO_BASE_URL "https://api.open-meteo.com/v1/forecast?latitude=%f&longitude=%f%s"

extern const char* METEO_CACHE_PATH;


/*============== Public functions ==============*/

int meteo_get_weather(float _lat, float _lon, bool _hourly);

/*==============================================*/

#endif
