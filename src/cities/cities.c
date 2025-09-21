#include "cities.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../utils/files.h"
#include "../utils/misc.h"
#include "../http/http.h"

/* -------------Internal function definitions---------------- */

void cities_parse_list(Cities* _cities, const char* _list);

/* ---------------------------------------------------------- */

int cities_init(Cities* _cities)
{
  const char* cities_list_string = load_file_as_string("data/city_coords.txt");
  if (cities_list_string == NULL) { 
    printf("Loading cities from file failed, exiting.\n"); 
    exit (0); 
  }

	memset(_cities, 0, sizeof(Cities));
	
	cities_parse_list(_cities, cities_list_string);

	meteo_init(&_cities->meteo, "https://api.open-meteo.com/v1/forecast");

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
		printf("  %i - %s, Latitude: %.4f, Longitude: %.4f\n", i+1, current->name, current->lat, current->lon);
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
}

int city_add(Cities* _cities, char* _name, float _lat, float _lon, City** _city)
{
	City* new_city = (City*)malloc(sizeof(City));
	if (new_city == NULL)
	{
		printf("Failed to allocate memory for new City\n");
		return -1;
	}

	new_city->name = _name;
	new_city->lat = _lat;
	new_city->lon = _lon;
	
	new_city->prev = NULL;
	new_city->next = NULL;

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
  
	if (_city->next == NULL && _city->prev == NULL)  /* I'm alone */
	{
		_cities->head = NULL;
		_cities->tail = NULL;
	}
	else if (_city->prev == NULL)                    /* I'm first */
	{
		_cities->head = _city->next;
		_city->next->prev = NULL;
	}
	else if (_city->next == NULL)                    /* I'm last */
	{
		_cities->tail = _city->prev;
		_city->prev->next = NULL;
	}
	else                                            /* I'm in the middle */
	{
		_city->prev->next = _city->next;
		_city->next->prev = _city->prev;
	}

	/* Alla utfall är hanterade, frigör minnet */

	free(_city);
}

int city_get_temperature(Cities* _cities, City* _city/* , float* _temperature */)
{
	return meteo_get_temperature(&_cities->meteo, _city->lat, _city->lon/* , _temperature */);
}

void cities_dispose(Cities* _c)
{
	_c++;
}
