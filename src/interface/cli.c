#include <stdio.h>

#include "cli.h"
#include "../utils/misc.h"

/* Prints question, takes y/Y/n/N as input, returns 0 for no and 1 for yes */
int yes_or_no(char* _question)
{
  char input[3];
  while (1)
  {
    printf("%s\n(Y/N): ", _question);
    fgets(input, sizeof(input), stdin);

    if (input[1] == '\n')
    {
      if (input[0] == 'y' || input[0] == 'Y')
        return 1;
      else if (input[0] == 'n' || input[0] == 'N')
        return 0;
    }
    else
    {
      printf("Ogiltigt svar!\n");
    }
    clear_input();
  }
}

/* Main CLI loop */
int cli_init(Cities* _cities) {

  int result;
  printf("Välkommen till Weatherio!\n\n");

  int go_again;
  do
  {
    go_again = 0;

    printf("Tillgängliga städer:\n");

    City* selected_city;
    int cities_count = cities_print(_cities); /* Print cities and get count */
    int input_choice;

    printf("\nVälj den stad du vill hämta väderleksdata ifrån: ");
    scanf("%5i", &input_choice);
    clear_input();

    if (input_choice <= cities_count && input_choice != 0)
    {
      result = city_get_by_index(_cities, &cities_count, &input_choice, &selected_city);
      if (result != 0) {
        printf("Failed to get chosen city, returned: %i\n", result);
        return (-2);
      }

      result = city_get_temperature(_cities, selected_city);
      if (result != 0) {
        printf("Failed to get temperature for city, returned: %i\n", result);
        return (-3);
      }

      /* printf("Du har valt: %s.\n", selected_city->name); */

    }
    else {
      printf("Ogiltigt svar! Välj en siffra...\n");
      input_choice = 0;
      go_again = 1;
    }

    if (go_again == 0)
    {
      go_again = yes_or_no("\nVill du välja en annan stad?");
    }

    printf("\n");

  } while (go_again != 0);

  printf("Ok, hejdå!\n");

  return 0;
}
