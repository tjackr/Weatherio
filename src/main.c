#include <stdio.h>

#include "cities/cities.h"
#include "http/http.h"
#include "helpers/misc.h"

#include <locale.h>


int main(void)
{
  int go_again = 1;
  char url[256];
  char response[512];
  char answer;

  setlocale(LC_ALL, "");

  printf("Välkommen!\nVar vänlig välj den stad du vill hämta väderleksdata ifrån:\n");

  while (go_again != 0)
  {
    cityurl(cities(), url);
    get_meteo_response(url, response);
    printf("JASON: %s\n", response);

    char question[37] = "Vill du söka en annan stad? (y/n): ";
    go_again = yes_or_no(question, answer);
    printf("go_again: %i\n", go_again);
  }

  return 0;
}
