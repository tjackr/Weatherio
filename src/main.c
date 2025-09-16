#include <stdio.h>

#include "cities/cities.h"
#include "http/http.h"
#include "helpers/misc.h"

#include <locale.h>


int main(void)
{
  int i;
  setlocale(LC_ALL, "en_US.UTF-8");  /* Believe it or not, atof() separates decimals based on locale, and our cities string uses dots to separate commas which is standard for US loacle. So choosing between replacing every dot in the string with commas and simply choosing en_US locale I chose the latter */

  /* int go_again = 1;
  char url[256];
  char response[512];
  char answer;


  printf("Välkommen!\nVar vänlig välj den stad du vill hämta väderleksdata ifrån:\n");

  while (go_again != 0)
  {
    cityurl(cities(), url);
    get_meteo_response(url, response);
    printf("JASON: %s\n", response);

    char question[37] = "Vill du söka en annan stad? (y/n): "  ;
    go_again = yes_or_no(question, answer);
    printf("go_again: %i\n", go_again);
  }
*/
  
  const char* citystring = "Stockholm:59.3293:18.0686\n" "Göteborg:57.7089:11.9746\n" "Malmö:55.6050:13.0038\n" "Uppsala:59.8586:17.6389\n" "Västerås:59.6099:16.5448\n" "Örebro:59.2741:15.2066\n" "Linköping:58.4109:15.6216\n" "Helsingborg:56.0465:12.6945\n" "Jönköping:57.7815:14.1562\n" "Norrköping:58.5877:16.1924\n" "Lund:55.7047:13.1910\n" "Gävle:60.6749:17.1413\n" "Sundsvall:62.3908:17.3069\n" "Umeå:63.8258:20.2630\n" "Luleå:65.5848:22.1567\n" "Kiruna:67.8558:20.2253\n"; 

  int city_count = line_count(citystring);


  city c[city_count];   
  
  citystring_parse(citystring, c);

  for (i = 0; i < city_count; i++)
  {
    printf("Name: %s; Latitude: %.4f; Longitude: %.4f\n", c[i].name, c[i].lat, c[i].lon);
  }

  /* We should read this from file using fopen in the future */

  

  return 0;
}
