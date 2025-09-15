#include <stdio.h>

#include "lib/cities.h"
#include "lib/http.h"


int yes_or_no(char* question, char answer)
{
  printf("%s\n", question);
  scanf("%c", &answer);

  if (answer == 'n' || answer == 'N')
  {
    return 0; /* No */
  }
  else if (answer == 'y' || answer == 'Y')
  {
    return 1; /* Yes */
  }
  else {
    return 2; /* Invalid */
  }
}

int main(void)
{
  int go_again = 1;
  char url[256];
  char response[512];

  printf("Välkommen!\nVar vänlig välj den stad du vill hämta väderleksdata ifrån:\n");

  do
  {
    cityurl(cities(), url);
    get_meteo_response(url, response);
    printf("JASON: %s\n", response);

    char question[37] = "Vill du söka en annan stad? (y/n): ";
    go_again = yes_or_no(question, go_again);
    printf("go_again: %i\n", go_again);


  } while (go_again != 0);

  return 0;
}
