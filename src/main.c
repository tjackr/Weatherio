#include <stdio.h>

#include "cities/cities.h"
#include "http/http.h"
#include "utils/misc.h"

#include <locale.h>


int main(void)
{
  int i;
  setlocale(LC_ALL, "en_US.UTF-8");  /* atof() separates decimals based on locale, and our cities string uses dots to separate commas which is standard for US locale. So choosing between replacing every dot in the string with commas and simply using en_US locale I chose the latter */

  
  /* We should read this from file using an fopen function in the future */
  const char* citystring = "Stockholm:59.3293:18.0686\n" "Göteborg:57.7089:11.9746\n" "Malmö:55.6050:13.0038\n" "Uppsala:59.8586:17.6389\n" "Västerås:59.6099:16.5448\n" "Örebro:59.2741:15.2066\n" "Linköping:58.4109:15.6216\n" "Helsingborg:56.0465:12.6945\n" "Jönköping:57.7815:14.1562\n" "Norrköping:58.5877:16.1924\n" "Lund:55.7047:13.1910\n" "Gävle:60.6749:17.1413\n" "Sundsvall:62.3908:17.3069\n" "Umeå:63.8258:20.2630\n" "Luleå:65.5848:22.1567\n" "Kiruna:67.8558:20.2253\n"; 

  int city_count = line_count(citystring);
  city cities[city_count];   
  
  citystring_parse(citystring, cities);

  printf("Välkommen till Weatherio!\n\nVälj den stad du vill hämta väderleksdata ifrån:\n");

  int go_again = 1;
/*
   char url[256];
  char response[512];
  city city;
*/
  while (go_again != 0)
  {

    
    for (i = 0; i < city_count; i++)
    {
      printf("  %i = %s (Latitude: %.4f, Longitude: %.4f)\n", i+1, cities[i].name, cities[i].lat, cities[i].lon);
    }

    printf("Val: ");
    int input_choice;
    scanf("%5i", &input_choice);
    clear_input();

    if (input_choice <= city_count && input_choice != 0)
    {
      printf("Du har valt: %s.\n", cities[input_choice-1].name);
      go_again = yes_or_no("Vill du köra igen? (y/n): ");
    }
    else {
      printf("Ogiltigt svar! Välj en siffra...\n");
      input_choice = 0;
    }

  }

}
