#include "cities.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../utils/files.h"
#include "../utils/misc.h"
#include "../includes/md5.h"
#include "../includes/tinydir.h"

/*============= Internal functions =============*/

void cities_parse_list(const char* _list);

void cities_parse_files(Cities* _Cities);

/*==============================================*/

const char* CITIES_PATH = "./data/cities/";
const char* CACHE_PATH = "./data/cities/";

int cities_init(Cities* _Cities)
{
  /* Create basic directories */
  if (create_directory_if_not_exists("./data/") != 0 ||
      create_directory_if_not_exists(CACHE_PATH) != 0 ||
      create_directory_if_not_exists(CITIES_PATH) != 0) 
  {
    printf("Failed to create data directories, exiting.\n");
    exit(0);
  }

  /* Null all values in Cities struct */
  memset(_Cities, 0, sizeof(Cities));

  /* Load cities from template file */
  FileString Cities_List_String = create_file_string("data/city_coords.txt");
  if (Cities_List_String.data == NULL) { 
    printf("Loading cities from file failed, exiting.\n"); 
    exit(1); 
  }

  /* Parse cities from template file and free string memory */
  cities_parse_list(Cities_List_String.data);
  destroy_file_string(&Cities_List_String);

  /* Parse cities from json files
   * This also adds each existing City to Cities */
  cities_parse_files(_Cities); 

  return 0;
}

/* Prints list of cities and returns their count */
int cities_print(Cities* _Cities)
{
	City* current = _Cities->head;
	if (current == NULL)
	{
		printf("No Cities to print\n");
		return -1;
	}

  int i = 0;
	do
	{
		printf("  %i. %s (%.4f, %.4f)\n", i+1, current->name, current->lat, current->lon);
		current = current->next;
    i++;

	} while (current != NULL);
  
  return i;
}

/* Populates our Cities struct, calling city_add based on template string
 * Format: Name:Lat:Lon\n */
void cities_parse_list(const char* _cities_string)
{
	char* list_copy = strdup(_cities_string);
	if (list_copy == NULL)
	{
		printf("Failed to allocate memory for list copy\n");
		return;
	}

	char* ptr = list_copy; /* Pointer that acts like cursor, walking through each character in string */

	char* name = NULL;
	char* lat_str = NULL;
	char* lon_str = NULL;
	do
	{
		if (name == NULL) /* First comes the name */
		{
			name = ptr;
		}
		else if (lat_str == NULL)
		{
			if (*ptr == ':') /* Then, when a colon appears we have the latitude */
			{
				lat_str = ptr + 1;
				*ptr = '\0';
			}
		}
		else if (lon_str == NULL)
		{
			if (*ptr == ':') /* On second colon we have the longitude */
			{
				lon_str = ptr + 1;
				*ptr = '\0';
			}
		}
		else
		{
			if (*ptr == '\n') /* Finally comes the newline, and we have all three parts */
			{
				*ptr = '\0';

        /* Check if city with same name already exists as json, skip adding it if so */
        char filepath[256];

        const char* hashed_name = MD5_HashToString(name, strlen(name));

        snprintf(filepath, sizeof(filepath), "%s%s.json", CITIES_PATH, hashed_name);

        if (!file_exists(filepath))
        {
          if (city_save_to_file(name, filepath, atof(lat_str), atof(lon_str)) != 0)
            printf("Failed to save %s to json..", name);
        }

        /* Reset variables */
				name = NULL;
				lat_str = NULL;
				lon_str = NULL;
			}
		}

		ptr++;

	} while (*ptr != '\0'); /* Unless next pointer is a null terminator we keep going */
	free(list_copy);
}

/* Reads all files in cities json directory for saved cities and adds them to Cities struct */
void cities_parse_files(Cities* _Cities)
{
  tinydir_dir dir;
  tinydir_open(&dir, CITIES_PATH);

  /* Loop through all files in dir */
  while (dir.has_next)
  {
    tinydir_file file;
    tinydir_readfile(&dir, &file);

    /* Only parse files with .json extension */
    if (strcmp( (file.name + strlen(file.name)) - strlen(".json"), ".json" ) == 0) 
    {
      char* full_path = stringcat(CITIES_PATH, file.name);

      json_t* json = json_load_from_file(full_path);

      json_t* json_name = json_object_get(json, "city_name");
      json_t* json_lat = json_object_get(json, "latitude");
      json_t* json_lon = json_object_get(json, "longitude");

      /* If above objects are fine we can add values to a new City struct */
      if (json_is_string(json_name) && json_is_real(json_lat) && json_is_real(json_lon))
      {
        city_add(_Cities, 
          strdup(json_string_value(json_name)), 
          (float)json_real_value(json_lat), 
          (float)json_real_value(json_lat), 
          NULL);
      }
      free(full_path);
    }
    tinydir_next(&dir);
  }
  tinydir_close(&dir);
}

/* Add new City struct to Cities struct */
int city_add(Cities* _Cities, char* _name, float _lat, float _lon, City** _City_Ptr)
{
	City* New_City = (City*)malloc(sizeof(City));

  if (New_City == NULL)
  {
    printf("Failed to allocate memory for new City\n");
    return -1;
  }

  New_City->name = strdup(_name);
  New_City->lat = _lat;
  New_City->lon = _lon;

  New_City->prev = NULL;
  New_City->next = NULL;

  /* Initialize as NULL, allocate when needed */
  New_City->weather = NULL;
  New_City->forecast = NULL;

  if (_Cities->tail == NULL) /* If no City added to Cities yet */
  {
    _Cities->head = New_City;
    _Cities->tail = New_City;
  }
  else
  {
    New_City->prev = _Cities->tail;
    _Cities->tail->next = New_City;
    _Cities->tail = New_City;
  }
  
  if (_City_Ptr != NULL)
    *_City_Ptr = New_City;

  return 0;
}

/* Get city by corresponding name */
int city_get_by_name(Cities* _Cities, const char* _name, City** _City_Ptr)
{
	City* Current = _Cities->head;
	if (Current == NULL)
		return -1;

	do
	{
		if (strcmp(Current->name, _name) == 0)
		{
			if (_City_Ptr != NULL)
				*_City_Ptr = Current;
				
			return 0;
		}

		Current = Current->next;

	} while (Current != NULL);
	
	return -2;
}

/* Get city by its index in the cities linked list */
int city_get_by_index(Cities* _Cities, int* _cities_count, int* _index, City** _City_Ptr)
{
  City* Current = _Cities->head;
  if (Current == NULL)
    return -1;

  int i;
  for (i = 0; i < *_cities_count; i++)
  {
    if (i == *_index-1)
    {
      *_City_Ptr = Current;
      return 0;
    }
    Current = Current->next;
  }

  return -2;
}

/* Remove City struct from Cities struct */
void city_remove(Cities* _Cities, City* _City)
{
  
	if (_City->next == NULL && _City->prev == NULL)  /* I'm alone :( */
	{
		_Cities->head = NULL;
		_Cities->tail = NULL;
	}
	else if (_City->prev == NULL)                    /* I'm first :) */
	{
		_Cities->head = _City->next;
		_City->next->prev = NULL;
	}
	else if (_City->next == NULL)                    /* I'm last :/ */
	{
		_Cities->tail = _City->prev;
		_City->prev->next = NULL;
	}
	else                                            /* I'm in the middle :| */
	{
		_City->prev->next = _City->next;
		_City->next->prev = _City->prev;
	}

	/* All cases are handled, free the memory */
	free(_City);
}

/* Creates json file containing city name and coordinates */
int city_save_to_file(const char* _filepath, const char* _city_name, float _lat, float _lon)
{
  /* Create new JSON with city info */
  json_t* city_data = json_object();
  json_object_set_new(city_data, "city_name", json_string(_city_name));
  json_object_set_new(city_data, "latitude", json_real(_lat));
  json_object_set_new(city_data, "longitude", json_real(_lon));

  /* Save json to file and cleanup jansson object */
  json_save_to_file(city_data, _filepath);
  json_decref(city_data);

  return 0;
}

/* Call meteo functions for given City's weather data */
int city_get_temperature(City* _City, bool _forecast)
{
  int result = -1;

  /* Allocate weather struct if not already allocated */
  if (_City->weather == NULL) {
    _City->weather = (Weather*)calloc(1, sizeof(Weather)); /* Do we free this anywhere? */
    if (_City->weather == NULL) {
      printf("Failed to allocate memory for weather data\n");
      return -1;
    }
  }

  /* Call meteo without direct mention of what City nor Cities is */
  json_t* full_weather_json = NULL;
  if (!_forecast)
  {
    result = meteo_get_current_weather(_City->lat, _City->lon, _City->weather, &full_weather_json);
    if (result == 0 && full_weather_json != NULL) 
    {
      json_decref(full_weather_json);
    }

    return result;
  }
  else
  {
    printf("meteo_get_forecast_weather() but unironically\n");

    return result;
  }
  
  printf("Failed to get weather...\n");
  return result;
}

/* Dispose of cities struct */
void cities_dispose(Cities* _c)
{
	_c++;
}
