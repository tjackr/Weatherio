#include "cities.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../utils/files.h"
#include "../utils/misc.h"
#include "../includes/md5.h"

/*============= Internal functions =============*/

void cities_parse_list(Cities* _cities, const char* _list);

void save_city_file(const char* city_name, float lat, float lon);

/*==============================================*/

int cities_init(Cities* _cities)
{
    /* Create basic directories */
    if (create_directory_if_not_exists("./data") != 0 ||
        create_directory_if_not_exists("./data/cache") != 0) {
        printf("Failed to create data directories\n");
    }

    memset(_cities, 0, sizeof(Cities));

    /* Fallback to text file if JSON doesn't exist */
    FileString cities_list_string = create_file_string("data/city_coords.txt");
    if (cities_list_string.data == NULL) { 
        printf("Loading cities from file failed, exiting.\n"); 
        exit(0); 
    }

    cities_parse_list(_cities, cities_list_string.data);
    destroy_file_string(&cities_list_string);

    return 0;
}

/* Prints list of cities and returns their count */
int cities_print(Cities* _cities)
{
	City* current = _cities->head;
	if (current == NULL)
	{
		printf("No Cities to print\n");
		return -1;
	}

  int i = 0;
	do
	{
		printf("  (%i) - %s, Latitude: %.4f, Longitude: %.4f\n", i+1, current->name, current->lat, current->lon);
		current = current->next;
    i++;

	} while (current != NULL);
  
  return i;

}

void cities_parse_list(Cities* _cities, const char* _cities_string)
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

				city_add(_cities, name, atof(lat_str), atof(lon_str), NULL);

				name = NULL;
				lat_str = NULL;
				lon_str = NULL;
			}
		}

		ptr++;

	} while (*ptr != '\0'); /* Unless next pointer is a null terminator we keep going */
	free(list_copy);
}

int city_add(Cities* _cities, char* _name, float _lat, float _lon, City** _city)
{
  /*
   * We might wanna add cities as json like:
   char* basepath = 'data/cities/'
   char filepath[(strlen(basepath) + strlen(_name) + strlen(".json"))]; 
   */

	City* new_city = (City*)malloc(sizeof(City));
    if (new_city == NULL)
    {
        printf("Failed to allocate memory for new City\n");
        return -1;
    }

    new_city->name = strdup(_name); /* Fix: create copy instead of pointing to stack */
    new_city->lat = _lat;
    new_city->lon = _lon;
    
save_city_file(new_city->name, new_city->lat, new_city->lon);

    new_city->prev = NULL;
    new_city->next = NULL;

    /* Initialize as NULL, allocate when needed */
    new_city->weather = NULL;
    new_city->forecast = NULL;

    if (_cities->tail == NULL)
    {
        _cities->head = new_city;
        _cities->tail = new_city;
    }
    else
    {
        new_city->prev = _cities->tail;
        _cities->tail->next = new_city;
        _cities->tail = new_city;
    }
    
    if (_city != NULL)
        *_city = new_city;

    return 0;
}

/* Get city by corresponding name */
int city_get_by_name(Cities* _cities, const char* _name, City** _city)
{
	City* current = _cities->head;
	if (current == NULL)
		return -1;

	do
	{
		if (strcmp(current->name, _name) == 0)
		{
			if (_city != NULL)
				*_city = current;
				
			return 0;
		}

		current = current->next;

	} while (current != NULL);
	
	return -2;
}

/* Get city by its index in the cities linked list */
int city_get_by_index(Cities* _cities, int* _cities_count, int* _index, City** _city)
{
  City* current = _cities->head;
  if (current == NULL)
    return -1;

  int i;
  for (i = 0; i < *_cities_count; i++)
  {
    if (i == *_index-1)
    {
      *_city = current;
      return 0;
    }
    current = current->next;
  }

  return -2;
}

void city_remove(Cities* _cities, City* _city)
{
  
	if (_city->next == NULL && _city->prev == NULL)  /* I'm alone :( */
	{
		_cities->head = NULL;
		_cities->tail = NULL;
	}
	else if (_city->prev == NULL)                    /* I'm first :) */
	{
		_cities->head = _city->next;
		_city->next->prev = NULL;
	}
	else if (_city->next == NULL)                    /* I'm last :/ */
	{
		_cities->tail = _city->prev;
		_city->prev->next = NULL;
	}
	else                                            /* I'm in the middle :| */
	{
		_city->prev->next = _city->next;
		_city->next->prev = _city->prev;
	}

	/* Alla utfall är hanterade, frigör minnet */

	free(_city);
}

int city_get_temperature(City* _city)
{
    /* Allocate weather struct if not already allocated */
    if (_city->weather == NULL) {
        _city->weather = (Weather*)calloc(1, sizeof(Weather));
        if (_city->weather == NULL) {
            printf("Failed to allocate memory for weather data\n");
            return -1;
        }
    }
    
    /* Call meteo without direct mention of what City nor Cities is */
	json_t* full_weather_json = NULL;
    int result = meteo_get_current_weather(_city->lat, _city->lon, _city->weather, &full_weather_json);
    
    /* Save weather data to city-specific file */
    if (result == 0 && full_weather_json != NULL) {
       /* json_t* weather_json = json_object();
        json_object_set_new(weather_json, "temperature", json_real(_city->weather->temperature));
        json_object_set_new(weather_json, "temperature_unit", json_string(_city->weather->temperature_unit));
        json_object_set_new(weather_json, "windspeed", json_real(_city->weather->windspeed));
        json_object_set_new(weather_json, "windspeed_unit", json_string(_city->weather->windspeed_unit));
        json_object_set_new(weather_json, "winddirection", json_real(_city->weather->winddirection));
        json_object_set_new(weather_json, "winddirection_unit", json_string(_city->weather->winddirection_unit));
        json_object_set_new(weather_json, "precipitation", json_real(_city->weather->precipitation));
        json_object_set_new(weather_json, "precipitation_unit", json_string(_city->weather->precipitation_unit));
        json_object_set_new(weather_json, "weather_code", json_integer(_city->weather->weather_code));

        save_city_weather_data(_city->name, _city->lat, _city->lon, weather_json); 
        json_decref(weather_json);*/
    	json_decref(full_weather_json);
    }
    
    return result;
}

void save_city_file(const char* city_name, float lat, float lon)
{
    create_directory_if_not_exists("./data/cities");
    
    char filepath[256];

    const char* hashed_city_name = MD5_HashToString(city_name, strlen(city_name));

    snprintf(filepath, sizeof(filepath), "./data/cities/%s.json", hashed_city_name);
    
    
    /* Create new JSON with city info */
    json_t* city_data = json_object();
    json_object_set_new(city_data, "city_name", json_string(city_name));
    json_object_set_new(city_data, "latitude", json_real(lat));
    json_object_set_new(city_data, "longitude", json_real(lon));
    

    
    json_save_to_file(city_data, filepath);
    json_decref(city_data);
}

/* Dispose of cities struct */
void cities_dispose(Cities* _c)
{
	_c++;
}
