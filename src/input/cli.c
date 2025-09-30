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
 * Option to add/remove new city for instance
 * */
int cli_init(Cities* _Cities) {

  int result;
  printf("\n\n"
    "Varmt välkommen till \n"
    "--------------------\n"
    "▖  ▖    ▗ ▌     ▘  ▌\n"
    "▌▞▖▌█▌▀▌▜▘▛▌█▌▛▘▌▛▌▌\n"
    "▛ ▝▌▙▖█▌▐▖▌▌▙▖▌ ▌▙▌▖\n"
    "--------------------\n\n\n"
    "Börja med att välja den stad du vill se väder från."
    "\n\n"
  );

  int go_again;
  do
  {
    go_again = 0;

    printf("Tillgängliga städer:\n");

    City* Selected_City;
    int cities_count = cities_print(_Cities); /* Print cities and get count */
    int input_choice = 0;

    printf("\nVälj stad: ");
    scanf("%5i", &input_choice);
    clear_input();

    if (input_choice <= cities_count && input_choice != 0)
    {
      result = city_get_by_index(_Cities, &cities_count, &input_choice, &Selected_City);
      if (result != 0) {
        printf("Något gick fel med att hämta tillgängliga städer! (Kod: %i)\n", result);
        return (-2);
      }

      result = city_get_temperature(Selected_City);
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
          Selected_City->name,
          Selected_City->weather->temperature,
          Selected_City->weather->temperature_unit,
          Selected_City->weather->windspeed,
          Selected_City->weather->windspeed_unit,
          Selected_City->weather->winddirection,
          Selected_City->weather->winddirection_unit,
          Selected_City->weather->precipitation,
          Selected_City->weather->precipitation_unit);
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
