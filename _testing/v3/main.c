#include <stdio.h>
#include "cities.h"

int main(void)
{
  int goAgain = 1;
  char url[256];
  char response[512];

  printf("Välkommen!\nVar vänlig välj den stad du vill hämta väderleksdata ifrån:\n");

  while (goAgain == 1)
  {
    cityurl(cities(), url);
    get_meteo_response(url, response);
    printf("Rått svar: %s\n", response);

    while (1)
    {
      char againRes;

      printf("Vill du söka en annan stad? (y/n): ");
      scanf("%c", &againRes);

      if (againRes == 'n' || againRes == 'N')
      {
        goAgain = 0;
        break;
      }
      else if (againRes != 'y' || againRes != 'Y')
      {
        goAgain = 1;
        break;
      }
    }
  }

  return 0;
}
