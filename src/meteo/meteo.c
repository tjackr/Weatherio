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

int meteo_build_url(char* _url, float _lat, float _lon, bool _forecast);

int meteo_define_filepath(char** _filepath, const char* _url);

int meteo_update_cache(const char* _filepath, const char* _url, bool _forecast);

int meteo_set_weather(const char* _filepath, Weather* _Weather, json_t** _full_json);

int meteo_set_forecast_weather(Forecast* _forecast, const char* _filepath);

time_t parse_time_string(const char* _time_str);

/*============== Global variables ==============*/

const char* CACHE_PATH = "./data/cache/";

/*==============================================*/

int meteo_build_url(char* _url, float _lat, float _lon, bool _forecast)
{
  char* params;

  if (_forecast)
    params = "&hourly=temperature_2m,relative_humidity_2m,apparent_temperature,is_day,precipitation,rain,showers,snowfall,weather_code,cloud_cover,pressure_msl,surface_pressure,wind_speed_10m,wind_direction_10m,wind_gusts_10m&timezone=auto";
  else
    params = "&current=temperature_2m,relative_humidity_2m,apparent_temperature,is_day,precipitation,rain,showers,snowfall,weather_code,cloud_cover,pressure_msl,surface_pressure,wind_speed_10m,wind_direction_10m,wind_gusts_10m&timezone=auto";

  /* Might wanna handle this more safely C89 style */
  sprintf(_url, 
    METEO_BASE_URL, 
    _lat, 
    _lon,
    params);

    printf("%s\n", _url);

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
int meteo_update_cache(const char* _filepath, const char* _url, bool _forecast)
{
  /* Check if cache exists
   * Then check update interval and timestamp from json to see if it should be updated */
	struct stat file_info;
	if (stat(_filepath, &file_info) == 0) 
	{
    int interval;
    char* str_timestamp;
    time_t now = time(NULL);

    /* If we're looking for current weather, use interval in json */
    if (!_forecast)
    {

      /* Parse update interval and timestamp from cache */
      json_t* json_current = json_object_get(json_load_from_file(_filepath), "current");
      json_t* json_interval = json_object_get(json_current, "interval");
      json_t* json_timestamp = json_object_get(json_current, "time");

      if (!json_is_integer(json_interval) || json_integer_value(json_interval) == 0) 
        return -1;
      interval = json_integer_value(json_interval);

      if (!json_is_string(json_timestamp) || json_string_value(json_timestamp) == NULL) 
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

      /* printf("Cache '%s' is '%.0lf' seconds old\n", _filepath, time_diff); */

      if (time_diff < (double)interval)
      {
        /*printf("Cache up to date\n");*/

        return 0; 
      }
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
  printf("%s\n", Http.addr);
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

int meteo_set_forecast_weather(Forecast* _forecast, const char* _filepath)
{
  printf("filepath: %s\n", _filepath);
  json_t* json = json_load_from_file(_filepath);
  if (json == NULL) 
  {
    printf("Failed to load JSON from file: %s\n", _filepath);
    return -1;
  }

  json_t* json_weather = json_object_get(json, "hourly");
  json_t* json_weather_units = json_object_get(json, "hourly_units");

  if (!json_weather ||  !json_weather_units) 
  {
    printf("Failed to get weather data objects from JSON\n");
    return -1;
  }

  json_t* json_temperature_unit = json_object_get(json_weather_units, "temperature_2m");
  json_t* json_windspeed_unit = json_object_get(json_weather_units, "wind_speed_10m");
  json_t* json_winddirection_unit = json_object_get(json_weather_units, "wind_direction_10m");
  json_t* json_precipitation_unit = json_object_get(json_weather_units, "precipitation");

  if (!json_is_string(json_temperature_unit) ||  
      !json_is_string(json_windspeed_unit) ||
      !json_is_string(json_winddirection_unit) ||
      !json_is_string(json_precipitation_unit))
  {
    printf("Missing or invalid units in JSON\n");
    return -2;
  }

  size_t index;
  json_t* json_times = json_object_get(json_weather, "time");
  json_t* json_temperature = json_object_get(json_weather, "temperature_2m");
  json_t* json_windspeed = json_object_get(json_weather, "wind_speed_10m");
  json_t* json_winddirection = json_object_get(json_weather, "wind_direction_10m");
  json_t* json_precipitation = json_object_get(json_weather, "precipitation");
  json_t* json_weathercode = json_object_get(json_weather, "weather_code");
  json_t* array_value;

  size_t hours = json_array_size(json_times);

  if (hours != 0) 
  {

    /* Allocate memory in Forecast for as many Weather structs we need */
    _forecast->weather = malloc(sizeof(Weather) * hours);

    /* Loop through all hours */
    json_array_foreach(json_times, index, array_value) {
      Weather hour_weather = {0};

      hour_weather.temperature_unit = json_string_value(json_temperature_unit);
      hour_weather.windspeed_unit = json_string_value(json_windspeed_unit);
      hour_weather.winddirection_unit = json_string_value(json_winddirection_unit);
      hour_weather.precipitation_unit = json_string_value(json_precipitation_unit);

      json_t* hour_time = json_array_get(json_times, index);
      json_t* hour_temperature = json_array_get(json_temperature, index);
      json_t* hour_windspeed = json_array_get(json_windspeed, index);
      json_t* hour_winddirection = json_array_get(json_winddirection, index);
      json_t* hour_precipitation = json_array_get(json_precipitation, index);
      json_t* hour_weathercode = json_array_get(json_weathercode, index);

      if (!json_is_number(hour_temperature) ||
        !json_is_number(hour_windspeed)     ||
        !json_is_number(hour_winddirection) ||
        !json_is_number(hour_precipitation) ||
        !json_is_integer(hour_weathercode)  )
      {
        printf("Missing or invalid weather data fields in JSON\n");
        return -3;
      }

      /* Timestamp */
      const char* str_timestamp = strdup(json_string_value(hour_time));
      time_t epoch = parse_time_string(str_timestamp);
      hour_weather.timestamp = epoch;
      /* printf("time_t: %li\n", hour_weather.timestamp); */
      
      /* Temperature */
      double temperature = json_real_value(hour_temperature);
      hour_weather.temperature = temperature;
      /* printf("temperature: %lf\n", hour_weather.temperature); */

      /* Windspeed */
      double windspeed = json_real_value(hour_windspeed);
      hour_weather.windspeed = windspeed;
      /* printf("windspeed: %lf\n", hour_weather.windspeed); */

      /* Winddirection */
      int winddirection = json_integer_value(hour_winddirection);
      hour_weather.winddirection = winddirection;
      /* printf("wind direction: %i\n", hour_weather.winddirection); */
      
      /* Precipitation */
      int precipitation = json_real_value(hour_precipitation);
      hour_weather.precipitation = precipitation;
      /* printf("Precipitation: %lf\n", hour_weather.precipitation); */
      
      /* Weather code */
      int weather_code = json_integer_value(hour_weathercode);
      hour_weather.weather_code = weather_code;
      /* printf("Weather code: %i\n", hour_weather.weather_code); */

      _forecast->weather[index] = hour_weather;

    }
  } else {
    printf("No hourly data found in JSON\n");
    return -5;
  }

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
int meteo_get_weather(float _lat, float _lon, Weather* _Weather, json_t** _full_json, bool _forecast)
{
  int result = 0;

  char url[512];
  result = meteo_build_url(url, _lat, _lon, _forecast);
  if (result != 0)
    return result;

  /* printf("URL built: %s\n", url); */

  char* filepath = NULL;
  result = meteo_define_filepath(&filepath, url);
  if (result != 0)
    return result;

  /* printf("Path built: %s\n", filepath); */

  result = meteo_update_cache(filepath, url, _forecast);
  if (result != 0)
    return result;

  Forecast forecast = {0};

  result = meteo_set_forecast_weather(&forecast, filepath);
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
