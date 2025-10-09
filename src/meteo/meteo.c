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

int meteo_build_url(char* _url, float _lat, float _lon, bool _hourly);

int meteo_define_filepath(char** _filepath, const char* _url);

int meteo_update_cache(const char* _filepath, const char* _url, bool _hourly);

int meteo_set_current_weather(const char* _filepath, Weather** _Weather_Ptr);

int meteo_set_forecast_weather(const char* _filepath, Forecast** _Forecast_Ptr);

/*============== Global variables ==============*/

const char* CACHE_PATH = "./data/cache/";

/*==============================================*/

int meteo_build_url(char* _url, float _lat, float _lon, bool _forecast)
{
  char* params;

  if (_forecast)
    params = "&hourly=temperature_2m,relative_humidity_2m,apparent_temperature,is_day,precipitation,rain,showers,snowfall,weather_code,cloud_cover,pressure_msl,surface_pressure,wind_speed_10m,wind_direction_10m,wind_gusts_10m&timezone=GMT";
  else
    params = "&current=temperature_2m,relative_humidity_2m,apparent_temperature,is_day,precipitation,rain,showers,snowfall,weather_code,cloud_cover,pressure_msl,surface_pressure,wind_speed_10m,wind_direction_10m,wind_gusts_10m&timezone=GMT";

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

/* Looks for cache file by name
 * Calls HTTP for new data if it doesn't exist or isn't recent enough */
int meteo_update_cache(const char* _filepath, const char* _url, bool _hourly)
{
  /* Check if cache exists */
	struct stat file_info;
	if (stat(_filepath, &file_info) == 0) 
	{
    char* str_timestamp;
    time_t now = time(NULL); /* NOTE: This could return local time on some systems */
    int interval;

    /* If we're looking for current weather, update interval will be defined in json */
    if (!_hourly)
    {
      /* Parse update interval and timestamp from json cache */
      json_t* json = json_load_from_file(_filepath);
      json_t* json_current = json_object_get(json, "current");
      json_t* json_interval = json_object_get(json_current, "interval");
      json_t* json_timestamp = json_object_get(json_current, "time");

      /* Valide and define */
      if (!json_is_string(json_timestamp) || json_string_value(json_timestamp) == NULL ||
          !json_is_integer(json_interval) || json_integer_value(json_interval) == 0)
      {
        json_decref(json);
        return -1;
      }

      interval = json_integer_value(json_interval);
      str_timestamp = strdup(json_string_value(json_timestamp));
      json_decref(json);
    } 
    else 
    {
      /* If forecast data, we want to see if earliest day in cache is before today
       * First timestamp in hourly meteo response is at midnight current day
       * so update interval will be 24 hours and reset every midnight */

      interval = 86400; /* epoch value for 24 hours */

      json_t* json = json_load_from_file(_filepath);
      json_t* json_hourly = json_object_get(json, "hourly");
      json_t* json_timestamps = json_object_get(json_hourly, "time");
      json_t* json_first_time = json_array_get(json_timestamps, 0);

      if (!json_is_string(json_first_time) || json_string_value(json_first_time) == NULL) 
      {
        json_decref(json);
        return -1;
      }

      str_timestamp = strdup(json_string_value(json_first_time));
      json_decref(json);
    }

    /* Define epoch timestamp and free duped string */
    time_t timestamp = parse_iso_datetime(str_timestamp);
    free(str_timestamp);

    if (timestamp == -1)
      return -2;

    double time_diff = difftime(now, timestamp);

    /* printf("Cache '%s' is '%.0lf' seconds old\n", _filepath, time_diff); */

    /* return if cache is already up to date */
    if (time_diff < (double)interval)
    {
      /*printf("Cache up to date\n");*/

      return 0; 
    }
  }

  /* Get new cache if it isn't recent */
  /* printf("Getting new cache (Hourly: %b)...\n", _hourly); */

	HTTP Http;
	if (http_init(&Http) != 0)
	{
		printf("Failed to initialize HTTP client\n");
		return -3;
	}

	int result = curl_get_response(&Http, _url);
  /* printf("%s\n", Http.addr); */
	if (result != 0)
	{
		printf("HTTP client failed to GET response. Errorcode: %i\n", result);
		http_dispose(&Http);
		return -4;
	}

  /* Validate JSON before saving */
  json_error_t error;
  json_t* json = json_loadb(Http.addr, Http.size, 0, &error);
  if (json == NULL) {
    printf("Invalid JSON received from API: %s\n", error.text);
    http_dispose(&Http);
    return -5;
  }

	result = write_string_to_file(Http.addr, _filepath);
	if (result != 0)
	{
		printf("Failed to write JSON to file: %i\n", result);
		http_dispose(&Http);
		return -6;
	}

  json_decref(json);
  return 0; 
}

/* Set current weather values to Weather struct from saved json cache */
int meteo_set_current_weather(const char* _filepath, Weather** _Weather_Ptr)
{
  /* Allocate weather struct if not already allocated */
  if (*_Weather_Ptr == NULL) {
    printf("Allocating memory for weather..\n");
    *_Weather_Ptr = (Weather*)calloc(1, sizeof(Weather)); 
    if (*_Weather_Ptr == NULL) {
      printf("Failed to allocate memory for weather struct\n");
      return -1;
    }
  }

  /* Call helper to load json root from file */
  json_t* json = json_load_from_file(_filepath);
  if (json == NULL) 
  {
    printf("Failed to load JSON from file: %s\n", _filepath);
    return -2;
  }

  /* Get main objects */
  json_t* current_weather = json_object_get(json, "current");
  json_t* current_weather_units = json_object_get(json, "current_units");

  if (!current_weather || !current_weather_units) 
  {
    printf("Failed to get weather data objects from JSON\n");
    json_decref(json);
    return -3;
  }

  /* Get weather values */
  json_t* json_temperature = json_object_get(current_weather, "temperature_2m");
  json_t* json_windspeed = json_object_get(current_weather, "wind_speed_10m");
  json_t* json_winddirection = json_object_get(current_weather, "wind_direction_10m");
  json_t* json_precipitation = json_object_get(current_weather, "precipitation");
  json_t* json_weather_code = json_object_get(current_weather, "weather_code");

  /* Get units */
  json_t* json_temperature_unit = json_object_get(current_weather_units, "temperature_2m");
  json_t* json_windspeed_unit = json_object_get(current_weather_units, "wind_speed_10m");
  json_t* json_winddirection_unit = json_object_get(current_weather_units, "wind_direction_10m");
  json_t* json_precipitation_unit = json_object_get(current_weather_units, "precipitation");

  /* Validate all expected fields */
  if (!json_is_number(json_temperature)   || !json_is_string(json_temperature_unit)   ||
      !json_is_number(json_windspeed)     || !json_is_string(json_windspeed_unit)     ||
      !json_is_number(json_winddirection) || !json_is_string(json_winddirection_unit) ||
      !json_is_number(json_precipitation) || !json_is_string(json_precipitation_unit) ||
      !json_is_integer(json_weather_code)) 
  {
    printf("Missing or invalid weather data fields in JSON\n");
    json_decref(json);
    return -4;
  }

  /* Duplicate string values
   * These need to be freed individually */
  const char* temperature_unit    = strdup(json_string_value(json_temperature_unit));
  const char* windspeed_unit      = strdup(json_string_value(json_windspeed_unit));
  const char* winddirection_unit  = strdup(json_string_value(json_winddirection_unit));
  const char* precipitation_unit  = strdup(json_string_value(json_precipitation_unit));

  /* Assign Weather members values */
  (*_Weather_Ptr)->temperature = json_number_value(json_temperature);
  (*_Weather_Ptr)->temperature_unit = temperature_unit;

  (*_Weather_Ptr)->windspeed = json_number_value(json_windspeed);
  (*_Weather_Ptr)->windspeed_unit = windspeed_unit;

  (*_Weather_Ptr)->winddirection = json_number_value(json_winddirection);
  (*_Weather_Ptr)->winddirection_unit = winddirection_unit;

  (*_Weather_Ptr)->precipitation = json_number_value(json_precipitation);
  (*_Weather_Ptr)->precipitation_unit = precipitation_unit;

  (*_Weather_Ptr)->weather_code = json_integer_value(json_weather_code);

  json_decref(json);

  return 0; 
}

int meteo_set_forecast_weather(const char* _filepath, Forecast** _Forecast_Ptr) 
{
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
    json_decref(json);
    return -1;
  }

  json_t* json_temperature_unit = json_object_get(json_weather_units, "temperature_2m");
  json_t* json_windspeed_unit = json_object_get(json_weather_units, "wind_speed_10m");
  json_t* json_winddirection_unit = json_object_get(json_weather_units, "wind_direction_10m");
  json_t* json_precipitation_unit = json_object_get(json_weather_units, "precipitation");

  if (!json_is_string(json_temperature_unit)   ||  
      !json_is_string(json_windspeed_unit)     ||
      !json_is_string(json_winddirection_unit) ||
      !json_is_string(json_precipitation_unit))
  {
    printf("Missing or invalid units in JSON\n");
    json_decref(json);
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
    /* _Forecast_Ptr->weather = malloc(sizeof(Weather) * hours); */
    *_Forecast_Ptr = (Forecast*)calloc(1, sizeof(Forecast)); 
    if (*_Forecast_Ptr == NULL) {
      printf("Failed to allocate memory for Forecast\n");
      json_decref(json);
      return -3;
    }

    /* Allocate memory in Forecast.weather for as many Weather structs we need */
    (*_Forecast_Ptr)->weather = (Weather*)calloc(1, sizeof(Weather) * hours); 
    if (*_Forecast_Ptr == NULL) {
      printf("Failed to allocate memory for Forecast weathers\n");
      json_decref(json);
      free(*_Forecast_Ptr);
      return -4;
    }

    /* We assume there is nothing in it now
     * Should look into possibility of there being data in it already at this stage
     * it should maybe be nulled if not null 
     * unless we want to keep a history of weather data but that seems like it could get too big for device's memory
     * Maybe add it during cache stage instead if so */
    (*_Forecast_Ptr)->count = -1;

    /* Loop through all hours to populate each Forecast->Weather */
    json_array_foreach(json_times, index, array_value) 
    {
      Weather hour_weather = {0};

      /* Duplicate string values
       * These need to be freed individually */
      const char* temperature_unit    = strdup(json_string_value(json_temperature_unit));
      const char* windspeed_unit      = strdup(json_string_value(json_windspeed_unit));
      const char* winddirection_unit  = strdup(json_string_value(json_winddirection_unit));
      const char* precipitation_unit  = strdup(json_string_value(json_precipitation_unit));

      hour_weather.temperature_unit   = temperature_unit;
      hour_weather.windspeed_unit     = windspeed_unit;
      hour_weather.winddirection_unit = winddirection_unit;
      hour_weather.precipitation_unit = precipitation_unit;

      json_t* hour_time = json_array_get(json_times, index);
      json_t* hour_temperature = json_array_get(json_temperature, index);
      json_t* hour_windspeed = json_array_get(json_windspeed, index);
      json_t* hour_winddirection = json_array_get(json_winddirection, index);
      json_t* hour_precipitation = json_array_get(json_precipitation, index);
      json_t* hour_weathercode = json_array_get(json_weathercode, index);

      if (!json_is_number(hour_temperature)   ||
          !json_is_number(hour_windspeed)     ||
          !json_is_number(hour_winddirection) ||
          !json_is_number(hour_precipitation) ||
          !json_is_integer(hour_weathercode)  )
      {
        printf("Missing or invalid weather data fields in JSON\n");
        json_decref(json);
        free((*_Forecast_Ptr)->weather);
        free(*_Forecast_Ptr);
        return -5;
      }

      /* Timestamp */
      const char* str_timestamp = strdup(json_string_value(hour_time));
      time_t epoch = parse_iso_datetime(str_timestamp);
      free((void*)str_timestamp);
      hour_weather.timestamp = epoch;
      /* printf("time_t: %li\n", hour_weather.timestamp); */

      /* Weather code */
      int weather_code = json_integer_value(hour_weathercode);
      hour_weather.weather_code = weather_code;
      /* printf("Weather code: %i\n", hour_weather.weather_code); */

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

      (*_Forecast_Ptr)->weather[index] = hour_weather;
    }

    /* Set forecast count to amount of hours after Weather populating success */
    (*_Forecast_Ptr)->count = hours;

  } else {
    printf("No hourly weather data found in cache file\n");
    return -5;
  }

  json_decref(json);

  return 0;
}

/* Calls necessary internal functions to get current and forecast weather */
int meteo_get_weather(float _lat, float _lon, Weather** _Weather_Ptr, Forecast** _Forecast_Ptr, bool _hourly)
{
  int result = 0;

  char url[512];
  result = meteo_build_url(url, _lat, _lon, _hourly);
  if (result != 0)
    return result;

  /* printf("URL built: %s\n", url); */

  char* filepath = NULL;
  result = meteo_define_filepath(&filepath, url);
  if (result != 0)
    return result;

  /* printf("Path built: %s\n", filepath); */

  result = meteo_update_cache(filepath, url, _hourly);
  if (result != 0)
    return result;

  if (!_hourly)
  {
    meteo_dispose(_Weather_Ptr, NULL); /* Make sure there is no old mem left */
    result = meteo_set_current_weather(filepath, _Weather_Ptr);
  } else {
    meteo_dispose(NULL, _Forecast_Ptr); /* Make sure there is no old mem left */
    result = meteo_set_forecast_weather(filepath, _Forecast_Ptr);
  }

  if (result != 0)
    return result;

  free(filepath);

  return result;
}

/* Dispose of these if they have been allocated:
* Weather (incl char* members)
* Forecast
* Forecast.Weather (incl. char* members) */
void meteo_dispose(Weather** _Weather_Ptr, Forecast** _Forecast_Ptr)
{
  if (_Weather_Ptr != NULL && *_Weather_Ptr != NULL)
  {
    free((void*)(*_Weather_Ptr)->temperature_unit);
    free((void*)(*_Weather_Ptr)->precipitation_unit);
    free((void*)(*_Weather_Ptr)->windspeed_unit);
    free((void*)(*_Weather_Ptr)->winddirection_unit);
    free(*_Weather_Ptr);
    *_Weather_Ptr = NULL;
  }

  if (_Forecast_Ptr != NULL && *_Forecast_Ptr != NULL)
  {
    if ((*_Forecast_Ptr)->weather != NULL)
    {
      int i;
      for (i = 0; i < (*_Forecast_Ptr)->count; i++)
      {
        free((void*)(*_Forecast_Ptr)->weather[i].temperature_unit);
        free((void*)(*_Forecast_Ptr)->weather[i].precipitation_unit);
        free((void*)(*_Forecast_Ptr)->weather[i].windspeed_unit);
        free((void*)(*_Forecast_Ptr)->weather[i].winddirection_unit);
      }
      free((*_Forecast_Ptr)->weather);
      (*_Forecast_Ptr)->weather = NULL;
    }

    free(*_Forecast_Ptr);
    *_Forecast_Ptr = NULL;
  }
}
