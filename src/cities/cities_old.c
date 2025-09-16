#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cities.h"
#include <ctype.h>





const char* cities_string = "Stockholm:59.3293:18.0686\n" "Göteborg:57.7089:11.9746\n" "Malmö:55.6050:13.0038\n" "Uppsala:59.8586:17.6389\n" "Västerås:59.6099:16.5448\n" "Örebro:59.2741:15.2066\n" "Linköping:58.4109:15.6216\n" "Helsingborg:56.0465:12.6945\n" "Jönköping:57.7815:14.1562\n" "Norrköping:58.5877:16.1924\n" "Lund:55.7047:13.1910\n" "Gävle:60.6749:17.1413\n" "Sundsvall:62.3908:17.3069\n" "Umeå:63.8258:20.2630\n" "Luleå:65.5848:22.1567\n" "Kiruna:67.8558:20.2253\n"; 

void format_name(char* input)
{
  int i;
  for (i = 0; i < (int)strlen(input); i++)
  {
    if (i == 0)
    {
      input[i] = toupper(input[i]);
    } else {
      input[i] = tolower(input[i]);
    }
  }
}

int line_count(char* string)
{
  int i;
  int lc = 0; 
  int len = strlen(string);
  for (i = 0; i < len; i++)
  {
    if (cities_string[i] == '\n')
      lc++;
  }
  return lc;
}

struct city* city_format(char* city_input, const char* cities_string)
{
  int city_count = 0;
  int offset = 0;
  int i;

  struct city* city_ptr = malloc(sizeof(city));
  if (!city_ptr) {
    printf("Failed to allocate memory for city_ptr");
  }

  /* Write cities to struct */
  /* int stop = 0; */
  /* while (offset < len && stop == 0) */
  for (i = 0; i < city_count; i++)
  {

    

    // sscanf(cities_string+offset, " %63[^:]:%15[^:]:%15[^\n]\n", city_ptr->name, city_ptr->lat, city_ptr->lon);

    /*Set offset to current city string length and two colons+\n */
    offset += strlen(city_ptr->name) + strlen(city_ptr->lat) + strlen(city_ptr->lon)+3;  
    /*
       printf("%s\n", city_input);
       printf("%s\n", city_ptr->name);
    */

    /* Format input string */
    format_name(city_input);

    if (strcmp(city_ptr->name, city_input) == 0)
    {
      city = city_ptr;
      printf("%s\n", city_ptr->name);
      printf("%s\n", city_ptr->lat);
      printf("%s\n", city_ptr->lon);
    }
  }
  return city_ptr;

}

int main(void) 
{
  char cities_input[64] = "stockholm";
  cities_format(cities_input, cities_string);
}
