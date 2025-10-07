#include <stdio.h>
#include <sys/stat.h>
#include <time.h>
#include <stdbool.h>

#include "meteo.h"
#include "../http/http.h"
#include "../includes/jansson/jansson.h"
#include "../includes/md5.h"
#include "../utils/files.h"
#include "../utils/misc.h"
#include "../utils/timeparse.h"

/*============= Internal functions =============*/

int meteo_build_url(char* _url, float _lat, float _lon, bool _current);

int meteo_define_filepath(char** _filepath, const char* _url);

int meteo_update_cache(const char* _filepath, const char* _url);

int meteo_set_current_weather(const char* _filepath, Weather* _Weather, json_t** _full_json);

time_t parse_time_string(const char* _time_str);

/*============== Global variables ==============*/

const char* CACHE_PATH = "./data/cache/";

/*==============================================*/

int meteo_build_url(char* _url, float _lat, float _lon, bool _current)
{
  char* params;

  if (_current)
    params = "&current=temperature_2m,relative_humidity_2m,apparent_temperature,is_day,precipitation,rain,showers,snowfall,weather_code,cloud_cover,pressure_msl,surface_pressure,wind_speed_10m,wind_direction_10m,wind_gusts_10m&timezone=auto";
  else
    params = "&hourly=temperature_2m,relative_humidity_2m,apparent_temperature,is_day,precipitation,rain,showers,snowfall,weather_code,cloud_cover,pressure_msl,surface_pressure,wind_speed_10m,wind_direction_10m,wind_gusts_10m&timezone=auto";

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

  /* Create cache directory if it doesn't exist */
  create_directory_if_not_exists(CACHE_PATH);

  /* Define the filepath for our json cache */
  int path_size = (strlen(CACHE_PATH) + strlen(hashed_url) + strlen(".json") + 1);
  *_filepath = (char*)malloc(sizeof(char) * path_size);

  strcpy(*_filepath, CACHE_PATH);
  strcat(*_filepath, hashed_url);
  strcat(*_filepath, ".json");

  return 0;
}

/* Check if we have the data in our cache folder, call API if not */
int meteo_update_cache(const char* _filepath, const char* _url)
{
  /* Check if cache exists
   * Then check update interval and timestamp from json to see if it should be updated */
	struct stat file_info;
	if (stat(_filepath, &file_info) == 0) 
	{
    int interval;
    char* str_timestamp;
    time_t now = time(NULL);

    /* Parse update interval and timestamp from cache */
    json_t* json_current = json_object_get(json_load_from_file(_filepath), "current");
    json_t* json_interval = json_object_get(json_current, "interval");
    json_t* json_timestamp = json_object_get(json_current, "time");

    if (!json_is_integer(json_interval) && json_integer_value(json_interval) == 0) 
      return -1;
    interval = json_integer_value(json_interval);

    if (!json_is_string(json_timestamp) && json_string_value(json_timestamp) == 0) 
      return -2;
    str_timestamp = strdup(json_string_value(json_timestamp));

    /* Eventually we should solve how to force UTC time instead of local time
     * But for now we'll just have to set timezone=auto to open-meteo
     * This lets them decide based on where user is sending request 
     * So uhh, turn off your VPNs boys */
    time_t timestamp = parse_time_string(str_timestamp);
    if (timestamp == -1)
      return -3;

		double time_diff = difftime(now, timestamp);

    /* 
    printf("stringtimestamp:%s\n", str_timestamp);
    printf("TimeNow: %i\n", (int)now);
    printf("timestamp:%i\n", (int)timestamp);
    printf("%lf\n", time_diff);
    */

    /* printf("Cache '%s' is '%.0lf' seconds old\n", _filepath, time_diff); */

		if (time_diff < (double)interval)
		{
      /*printf("Cache up to date\n");*/

      return 0; 
    }
  }

  /* Get new cache if it isn't recent */
  /* printf("Getting new cache... \n"); */

	HTTP Http;
	if (http_init(&Http) != 0)
	{
		printf("Failed to initialize HTTP\n");
		return -1;
	}

	int result = curl_get_response(&Http, _url);
	if (result != 0)
	{
		printf("HTTP GET request failed. Errorcode: %i\n", result);
		http_dispose(&Http);
		return -2;
	}

  /* Validate JSON before saving */
  json_error_t error;
  json_t* json = json_loadb(Http.addr, Http.size, 0, &error);
  if (json == NULL) {
    printf("Invalid JSON received from API: %s\n", error.text);
    http_dispose(&Http);
    return -3;
  }

	result = write_string_to_file(Http.addr, _filepath);
	if (result != 0)
	{
		printf("Failed to write JSON to file: %i\n", result);
		http_dispose(&Http);
		return -4;
	}

  json_decref(json);
  return 0; 
}

/* Set current weather values to City's Meteo's Weather struct*/
int meteo_set_current_weather(const char* _filepath, Weather* _Weather, json_t** _full_json)
{
  json_t* json = json_load_from_file(_filepath);
  if (json == NULL) 
  {
    printf("Failed to load JSON from file: %s\n", _filepath);
    return -1;
  }

  /* Get main objects */
  json_t* current_weather = json_object_get(json, "current");
  json_t* current_weather_units = json_object_get(json, "current_units");

  if (!current_weather || !current_weather_units) 
  {
    printf("Failed to get weather data objects from JSON\n");
    json_decref(json);
    return -2;
  }

  /* Get weather values */
  json_t* temperature = json_object_get(current_weather, "temperature_2m");
  json_t* windspeed = json_object_get(current_weather, "wind_speed_10m");
  json_t* winddirection = json_object_get(current_weather, "wind_direction_10m");
  json_t* precipitation = json_object_get(current_weather, "precipitation");
  json_t* weather_code = json_object_get(current_weather, "weather_code");

  /* Get units */
  json_t* temperature_unit = json_object_get(current_weather_units, "temperature_2m");
  json_t* windspeed_unit = json_object_get(current_weather_units, "wind_speed_10m");
  json_t* winddirection_unit = json_object_get(current_weather_units, "wind_direction_10m");
  json_t* precipitation_unit = json_object_get(current_weather_units, "precipitation");

  /* Validate all required fields are present */
  if (!json_is_number(temperature) || !json_is_string(temperature_unit) ||
    !json_is_number(windspeed) || !json_is_string(windspeed_unit) ||
    !json_is_number(winddirection) || !json_is_string(winddirection_unit) ||
    !json_is_number(precipitation) || !json_is_string(precipitation_unit) ||
    !json_is_integer(weather_code)) 
  {
    printf("Missing or invalid weather data fields in JSON\n");
    json_decref(json);
    return -3;
  }

  /* Set weather struct values */
  _Weather->temperature = json_number_value(temperature);
  _Weather->temperature_unit = strdup(json_string_value(temperature_unit));

  _Weather->windspeed = json_number_value(windspeed);
  _Weather->windspeed_unit = strdup(json_string_value(windspeed_unit));

  _Weather->winddirection = json_number_value(winddirection);
  _Weather->winddirection_unit = strdup(json_string_value(winddirection_unit));

  _Weather->precipitation = json_number_value(precipitation);
  _Weather->precipitation_unit = strdup(json_string_value(precipitation_unit));

  _Weather->weather_code = json_integer_value(weather_code);
  
  if (_full_json != NULL) 
  {
    *_full_json = json;
    json_incref(json); /* Increase reference count so it doesn't get freed */
  }

  /* Clean up */
  json_decref(json);

  return 0; 
}

/* Main public function for calling necessary functions needed to get individal City weather */
int meteo_get_current_weather(float _lat, float _lon, Weather* _Weather, json_t** _full_json)
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

  result = meteo_set_current_weather(filepath, _Weather, _full_json);
  if (result != 0)
    return result;

  free(filepath);

  return result;
}

/* 
   Should exist:
   int meteo_get_forecast(float _lat, float _lon, Weather* _weather) 
*/
