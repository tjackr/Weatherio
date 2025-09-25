#include <stdio.h>
#include <sys/stat.h>
#include <time.h>
#include <stdbool.h>

#include "meteo.h"
#include "../http/http.h"
#include "../includes/cJSON.h"
#include "../includes/md5.h"
#include "../utils/files.h"
#include "../utils/misc.h"

/*============= Internal functions =============*/

int meteo_build_url(char* _url, float _lat, float _lon, bool _current);

int meteo_define_filepath(char** _filepath, const char* _url);

int meteo_update_cache(const char* _filepath, const char* _url);

int meteo_set_current_weather(const char* _filepath, Weather* _weather);

/*==============================================*/

int meteo_build_url(char* _url, float _lat, float _lon, bool _current)
{
  /* ---Unfinished
   * We could build the meteo url dynamically with a set of parameters to get specific data
   * User should set options in a seperate function beforehand, either into the meteo struct directly or into an options struct 
   *
   * Altough it's better to get as much data as possible at once from the API
   * Unless we want to get forecast then we should switch atleast one param
   *
   Example parameters:
   hourly=
   daily=
   current=
   current_weather=(true || fase)
   temperature_unit=(fahrenheit || celsius)

   Resource: https://open-meteo.com/en/docs#api_documentation
   * */

  /* sprintf(url, "%s?latitude=%.4f&longitude=%.4f&current_weather=true", _meteo->api_url, _lat, _lon); */
  /* sprintf(url, "%s?latitude=%f&longitude=%f&hourly=temperature_2m", _meteo->api_url, _lat, _lon); */
	/* sprintf(url, sizeof(url), "%s/v1/forecast?latitude=%2.2f&longitude=%2.2f&hourly=temperature_2m", _meteo->api_url, _lat, _lon); */

  char* params;

  if (_current)
    params = "&current=temperature_2m,relative_humidity_2m,apparent_temperature,is_day,precipitation,rain,showers,snowfall,weather_code,cloud_cover,pressure_msl,surface_pressure,wind_speed_10m,wind_direction_10m,wind_gusts_10m";
  else
    params = "&hourly=temperature_2m,relative_humidity_2m,apparent_temperature,is_day,precipitation,rain,showers,snowfall,weather_code,cloud_cover,pressure_msl,surface_pressure,wind_speed_10m,wind_direction_10m,wind_gusts_10m";

  /* Might wanna handle this more safely C89 style */
  sprintf(_url, 
      METEO_BASE_URL, 
      _lat, 
      _lon,
      params);

  return 0;
}

/* Defines a path name to a cache file based on the url used */
int meteo_define_filepath(char** _filepath, const char* _url)
{
  const char* hashed_url = MD5_HashToString(_url, strlen(_url));

  /* Define the filepath for our json cache */
  char* basepath = "./data/cache/";
  int path_size = (strlen(basepath) + strlen(hashed_url) + strlen(".json"));
  *_filepath = (char*)malloc(sizeof(char) * path_size); /* We calculate the size of filepath */

  strcpy(*_filepath, basepath);
  strcat(*_filepath, hashed_url);
  strcat(*_filepath, ".json");

  return 0;
}

/* See if we have weather cache, call API if not */
int meteo_update_cache(const char* _filepath, const char* _url)
{

  /* Check if cache exists and is recent enough */
  /* This should check timestamp based on json instead
   * http could check stat instead (if we want it to be responsible for files directly) */
	struct stat file_info;
	if (stat(_filepath, &file_info) == 0)
	{
		time_t now = time(NULL);
		double time_diff = difftime(now, file_info.st_mtime);

    /* printf("Cache '%s' is '%.0lf' seconds old\n", _filepath, time_diff); */

		if (time_diff < 900)
		{
      /* printf("Cache up to date\n"); */

      return 0; 
    }
  }

  /* And get new cache if it isn't recent */
  /* printf("Getting new cache... \n"); */

	HTTP http;
	if (http_init(&http) != 0)
	{
		printf("Failed to initialize HTTP\n");
		return -1;
	}

	int result = curl_get_response(&http, _url);
	if (result != 0)
	{
		printf("HTTP GET request failed. Errorcode: %i\n", result);
		http_dispose(&http);
		return -2;
	}

  /*  
   * THIS SHOULD PROBABLY BE PARSED FIRST
   * */

	result = write_string_to_file(http.addr, _filepath);
	if (result != 0)
	{
		printf("Failed to write JSON to file: %i\n", result);
		return -3;
	}


  return 0; 
}

/* Set current weather values to City's Meteo's Weather struct*/
int meteo_set_current_weather(const char* _filepath, Weather* _weather)
{
  /* This is very quick and dirty for now
   *
   * It segfaults and shits the bed if it can't find the correct json object
   *
   * Should make a proper for loop through these...
   * probably best to use cJSON array functionality to dynamically build entire json tree
   * then look for certain keywords and match them with Weather struct members somehow
   *
   * Any way to make this look cleaner is appreciated
   */
  
  /* Declare our objects */
  cJSON* json = NULL;
  const cJSON* current_weather = NULL;
  const cJSON* temperature = NULL;
  const cJSON* current_weather_units = NULL;
  const cJSON* temperature_unit = NULL;
  const cJSON* timestamp = NULL;
  const cJSON* weathercode = NULL;
  const cJSON* windspeed = NULL;
  const cJSON* windspeed_unit = NULL;
  const cJSON* winddirection = NULL;
  const cJSON* winddirection_unit = NULL;
  const cJSON* precipitation = NULL;
  const cJSON* precipitation_unit = NULL;

  /* Parse the json file */
  FileString json_file = create_file_string(_filepath);
  json_parse(&json, json_file.data, json_file.size);
  destroy_file_string(&json_file);

  if (json == NULL)
  {
    printf("Failed to parse meteo JSON root\n");
    cJSON_Delete(json);
    return -1;
  }
  
  /* Get parents */
  current_weather = cJSON_GetObjectItemCaseSensitive(json, "current");
  current_weather_units = cJSON_GetObjectItemCaseSensitive(json, "current_units");

  if (current_weather == NULL ||
      current_weather_units == NULL)
  {
    printf("Failed to get meteo json parents\n");
    cJSON_Delete(json);
    return -2;
  }

  /* Get children */
  timestamp = cJSON_GetObjectItemCaseSensitive(current_weather, "time");
  weathercode = cJSON_GetObjectItemCaseSensitive(current_weather, "weather_code");
  temperature = cJSON_GetObjectItemCaseSensitive(current_weather, "temperature_2m");
  windspeed = cJSON_GetObjectItemCaseSensitive(current_weather, "wind_speed_10m");
  winddirection = cJSON_GetObjectItemCaseSensitive(current_weather, "wind_direction_10m");
  precipitation = cJSON_GetObjectItemCaseSensitive(current_weather, "precipitation");

  temperature_unit = cJSON_GetObjectItemCaseSensitive(current_weather_units, "temperature_2m");
  windspeed_unit = cJSON_GetObjectItemCaseSensitive(current_weather_units, "wind_speed_10m");
  winddirection_unit = cJSON_GetObjectItemCaseSensitive(current_weather_units, "wind_direction_10m");
  precipitation_unit = cJSON_GetObjectItemCaseSensitive(current_weather_units, "precipitation");

  if (current_weather == NULL ||
    temperature == NULL ||
    current_weather_units == NULL ||
    temperature_unit == NULL ||
    timestamp == NULL ||
    weathercode == NULL ||
    windspeed == NULL ||
    windspeed_unit == NULL ||
    winddirection == NULL ||
    winddirection_unit == NULL ||
    precipitation == NULL ||
    precipitation_unit == NULL)
  {
    printf("Failed to get meteo json children\n");
    cJSON_Delete(json);
    return -3;
  }

  /* Should also use proper handling for these using cJSON_IsString etc. */
  _weather->temperature = temperature->valuedouble;
  _weather->temperature_unit = strdup(temperature_unit->valuestring);

  _weather->windspeed = windspeed->valuedouble;
  _weather->windspeed_unit = strdup(windspeed_unit->valuestring);

  /* Would be cool also to have a function that sets named direction like northeast for these */
  _weather->winddirection = winddirection->valuedouble;
  _weather->winddirection_unit = strdup(winddirection_unit->valuestring);

  _weather->precipitation = precipitation->valuedouble;
  _weather->precipitation_unit = strdup(precipitation_unit->valuestring);

  _weather->timestamp = 0; /* Should parse time to seconds for proper stamping */
  _weather->weather_code = weathercode->valueint;

  /* Clean up cJSON */
  cJSON_Delete(json);

  return 0; 
}

int meteo_get_current_weather(float _lat, float _lon, Weather* _weather)
{
  int result = 0;

  char url[512];
  result = meteo_build_url(url, _lat, _lon, true);
  if (result != 0)
    return result;

  /* printf("URL built: %s\n", url); */

  char* filepath = NULL;
  result = meteo_define_filepath(&filepath, url);
  if (result != 0)
    return result;

  /* printf("Path built: %s\n", filepath); */

  result = meteo_update_cache(filepath, url);
  if (result != 0)
    return result;

  result = meteo_set_current_weather(filepath, _weather);
  if (result != 0)
    return result;

  free(filepath);

  return result;
}

/* 
   Should exist:
   int meteo_get_forecast(float _lat, float _lon, Weather* _weather) 
*/
