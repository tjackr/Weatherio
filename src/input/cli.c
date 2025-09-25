#include <stdio.h>

#include "cli.h"
#include "../utils/misc.h"

/*============= Internal functions =============*/

int yes_or_no(char* _question);

/*==============================================*/

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
/* Feel free to add fancier menus and more choices
 * Maybe more like a main menu with options
 * Option to add new city for instance
 * Also remove, so we can rid the world of malmö
 * */
int cli_init(Cities* _cities) {

  int result;
  printf(
    "Varmt välkommen till \n"
    "--------------------\n"
    "▖  ▖    ▗ ▌     ▘  ▌\n"
    "▌▞▖▌█▌▀▌▜▘▛▌█▌▛▘▌▛▌▌\n"
    "▛ ▝▌▙▖█▌▐▖▌▌▙▖▌ ▌▙▌▖\n"
    "--------------------\n\n\n"
    "Börja med att välja den stad du vill se väder från.\n\n"
  );

  int go_again;
  do
  {
    go_again = 0;

    printf("Tillgängliga städer:\n");

    City* selected_city;
    int cities_count = cities_print(_cities); /* Print cities and get count */
    int input_choice = 0;

    printf("\nVälj stad: ");
    scanf("%5i", &input_choice);
    clear_input();

    if (input_choice <= cities_count && input_choice != 0)
    {
      result = city_get_by_index(_cities, &cities_count, &input_choice, &selected_city);
      if (result != 0) {
        printf("Något gick fel med att hämta tillgängliga städer! (Kod: %i)\n", result);
        return (-2);
      }

      result = city_get_temperature(selected_city);
      if (result != 0) {
        printf("Något gick fel med att hämta väderdata! (Kod: %i)\n", result);
      }
      else
      {
        printf("\n"
          "*** Vädret i %s: \n"
          "**\n"
          "*   Temperatur: %.1lf %s\n"
          "*   Vindstyrka: %.1lf %s\n"
          "**  Vindriktning: %.0lf %s\n"
          "*** Nederbörd: %.2lf%s\n" 
          "***********************\n\n",
          selected_city->name,
          selected_city->weather->temperature,
          selected_city->weather->temperature_unit,
          selected_city->weather->windspeed,
          selected_city->weather->windspeed_unit,
          selected_city->weather->winddirection,
          selected_city->weather->winddirection_unit,
          selected_city->weather->precipitation,
          selected_city->weather->precipitation_unit);
      }
    }
    else {
      printf("\nOgiltigt svar! Välj en siffra ur listan...");
      input_choice = 0;
      go_again = 1;
    }

    if (go_again == 0)
    {
      go_again = yes_or_no("\nVill du välja en ny stad?");
    }

    printf("\n");

  } while (go_again != 0);

  printf("Ok, hejdå!\n");

  return 0;
}
