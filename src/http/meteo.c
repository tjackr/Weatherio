#include <stdio.h>

#include "meteo.h"
#include "http.h"

int meteo_init(Meteo* _meteo, const char* _api_url)
{
	memset(_meteo, 0, sizeof(Meteo));

	_meteo->api_url = _api_url;
	
	return 0;
}

int meteo_build_url(Meteo* _meteo)
{
  /* ---Unfinished
   * We should build the meteo url dynamically with a set of parameters to get specific data
   * User should set options in a seperate function beforehand, either into the meteo struct directly or into an options struct 
   *
   Example parameters:
   hourly=
   daily=
   current=
   current_weather=(true || fase)
   temperature_unit=(fahrenheit || celsius)
   * */
  
  char* finished_url;
  char* base_url = "https://api.open-meteo.com/v1/forecast"; 
  finished_url = base_url;

  _meteo->api_url = finished_url;

	/* sprintf(url, sizeof(url), "%s/v1/forecast?latitude=%2.2f&longitude=%2.2f&hourly=temperature_2m", _meteo->api_url, _lat, _lon); */
  return 0;
}

int meteo_get_temperature(Meteo* _meteo, float _lat, float _lon/* , float* _temperature */)
{
	HTTP http;
	if (http_init(&http) != 0)
	{
		printf("Failed to initialize HTTP\n");
		return -1;
	}

	char url[512];

	sprintf(url, "%s?latitude=%f&longitude=%f&current_weather=true", _meteo->api_url, _lat, _lon);
	int result = curl_get_response(&http, url);
	if (result != 0)
	{
		printf("HTTP GET request failed. Errorcode: %i\n", result);
		http_dispose(&http);
		return -2;
	}


  /* Here we should call our json parser to get current temperature */
	/* *(_temperature) = 0; */

	return 0;
}

void meteo_dispose(Meteo* _m)
{
  
  _m++;
}
