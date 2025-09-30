#include <stdio.h>
#include <stdlib.h>
#include <locale.h>


#include "cities/cities.h"
#include "input/cli.h"

int main(void)
{
  int result;

  setlocale(LC_ALL, "en_US.UTF-8");  /* atof() separates decimals based on locale, and the cities string uses dots to separate decimals which is standard for US loacle. So choosing between replacing every dot in the string with commas and simply choosing en_US locale I chose the latter */
  Cities Cities;
  
  /* Initialize cities backend */
  result = cities_init(&Cities);
  if (result != 0)
  {
    printf("Failed to initialize cities, returned: %i\n", result);
    exit(-1);
  }

  /* Initialize CLI interface */
  result = cli_init(&Cities);
  if (result != 0)
  {
    printf("CLI interface failed, returned: %i\n", result);
    cities_dispose(&Cities);
    exit(-2);
  }

  cities_dispose(&Cities);

  return 0;
}
