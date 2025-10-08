#include <stdio.h>
#include <string.h>
#include <time.h>

#include "cli.h"
#include "../utils/misc.h"
#include "../utils/weather_codes.h"
#include "../utils/timeparse.h"

/*============= Internal functions =============*/

int display_menu(void);

static int utf8_display_width(const char* _str);

static void print_fixed_line(const char* _text, int _width);

/*==============================================*/

static int utf8_display_width(const char* _str) {
  int width = 0;
  const unsigned char *s = (const unsigned char*)_str;
  
  while (*s) {
    if ((*s & 0x80) == 0) {
      /* 1-byte ASCII (0xxxxxxx) */
      width++;
      s++;
    } else if ((*s & 0xE0) == 0xC0) {
      /* 2-byte UTF-8 (110xxxxx 10xxxxxx) */
      width++;
      s += 2;
    } else if ((*s & 0xF0) == 0xE0) {
      /* 3-byte UTF-8 (1110xxxx 10xxxxxx 10xxxxxx) */
      width++;
      s += 3;
    } else if ((*s & 0xF8) == 0xF0) {
      /* 4-byte UTF-8 (11110xxx 10xxxxxx 10xxxxxx 10xxxxxx) */
      width++;
      s += 4;
    } else {
      /* Invalid byte - skip */
      s++;
    }
  }
  
  return width;
}

/* Function to print a string with fixed width */
/* Add this function after utf8_display_width */
static void print_fixed_line(const char* _text, int _width) 
{
  int vis_len = utf8_display_width(_text);
  
  /* If the text is too long - truncate it */
  if (vis_len > _width) {
    const unsigned char* s = (const unsigned char*)_text;
    int printed = 0;
    
    printf("│");
    while (*s && printed < _width) {
      if ((*s & 0x80) == 0) {
        /* ASCII */
        putchar(*s);
        s++;
        printed++;
      } else if ((*s & 0xE0) == 0xC0) {
        /* 2-byte UTF-8 */
        if (printed + 1 <= _width) {
          putchar(*s++);
          putchar(*s++);
          printed++;
        } else break;
      } else if ((*s & 0xF0) == 0xE0) {
        /* 3-byte UTF-8 */
        if (printed + 1 <= _width) {
          putchar(*s++);
          putchar(*s++);
          putchar(*s++);
          printed++;
        } else break;
      } else if ((*s & 0xF8) == 0xF0) {
        /* 4-byte UTF-8 */
        if (printed + 1 <= _width) {
          putchar(*s++);
          putchar(*s++);
          putchar(*s++);
          putchar(*s++);
          printed++;
        } else break;
      } else {
          s++;
      }
    }
    
    /* Fill the rest with spaces */
    for (int i = printed; i < _width; i++) {
      putchar(' ');
    }
    printf("║\n");
  } else {
    /* If the text fits - print with padding */
    printf("║%s", _text);
    for (int i = vis_len; i < _width; i++) {
      putchar(' ');
    }
    printf("║\n");
  }
}

/* Display main menu and return user choice */
int display_menu(void)
{
  int choice;
  printf("\n=== HUVUDMENY ===\n");
  printf("1. Visa väder för en stad\n");
  printf("0. Avsluta programmet\n");
  printf("Välj alternativ: ");
  
  scanf("%5i", &choice);
  clear_input();
  
  return choice;
}

/* Main CLI loop */
int cli_init(Cities* _Cities) {

  int result;
  printf("\n\n"
    "Varmt välkommen till \n"
    "--------------------\n"
    "▖  ▖    ▗ ▌     ▘  ▌\n"
    "▌▞▖▌█▌▀▌▜▘▛▌█▌▛▘▌▛▌▌\n"
    "▛ ▝▌▙▖█▌▐▖▌▌▙▖▌ ▌▙▌▖\n"
    "--------------------\n\n\n"
  );

  int running = 1;
  
  while (running)
  {
    int menu_choice = display_menu();
    
    switch (menu_choice)
    {
      case 1: /* Visa väderdata */
      {
        printf("\nTillgängliga städer:\n");

        City* Selected_City;
        int cities_count = cities_print(_Cities);
        int input_choice = 0;

        printf("\nVälj stad: ");
        scanf("%5i", &input_choice);
        clear_input();

        if (input_choice <= cities_count && input_choice != 0)
        {
          result = city_get_by_index(_Cities, &cities_count, &input_choice, &Selected_City);
          if (result != 0) {
            printf("Något gick fel med att hämta tillgängliga städer! (Kod: %i)\n", result);
            break;
          }

          /* Get current weather */
          result = city_get_weather(Selected_City, false);
          printf("city_get_wether result: %i\n", result);
          printf("weather code: %i\n", Selected_City->weather->weather_code);
          if (result != 0) {
            printf("Något gick fel med att hämta väderdata! (Kod: %i)\n", result);
          }
          else
          {
            printf("weather code: %i\n", Selected_City->weather->weather_code);
            const char* description = get_weather_description(Selected_City->weather->weather_code);
            const char* ascii = get_weather_ascii_art(Selected_City->weather->weather_code);
            
            const int CONTENT_WIDTH = 42;
            
            printf("\n");
            printf("╔══════════════════════════════════════════╗\n");
            
            /* Header */
            char header_buffer[128];
            snprintf(header_buffer, sizeof(header_buffer), "  Väder för: %s", Selected_City->name);
            print_fixed_line(header_buffer, CONTENT_WIDTH);
            
            printf("╠══════════════════════════════════════════╣\n");
            
            /* ASCII art */
            char ascii_copy[512];
            strncpy(ascii_copy, ascii, sizeof(ascii_copy) - 1);
            ascii_copy[sizeof(ascii_copy) - 1] = '\0';
            
            char* line = strtok(ascii_copy, "\n");
            while (line != NULL) {
              char ascii_line[128];
              snprintf(ascii_line, sizeof(ascii_line), " %s ", line);
              print_fixed_line(ascii_line, CONTENT_WIDTH);
              line = strtok(NULL, "\n");
            }
            
            /* Empty line */
            print_fixed_line("", CONTENT_WIDTH);
            
            /* Weather description */
            char desc_buffer[128];
            snprintf(desc_buffer, sizeof(desc_buffer), "  %s", description);
            print_fixed_line(desc_buffer, CONTENT_WIDTH);
            
            /* Empty line */
            print_fixed_line("", CONTENT_WIDTH);
            
            /* Weather data */
            char line_buffer[128];
            
            
            snprintf(line_buffer, sizeof(line_buffer), "  Temperatur:    %.1lf %s", 
              Selected_City->weather->temperature,
              Selected_City->weather->temperature_unit);
            print_fixed_line(line_buffer, CONTENT_WIDTH);
            
            snprintf(line_buffer, sizeof(line_buffer), "  Vindstyrka:    %.1lf %s",
              Selected_City->weather->windspeed,
              Selected_City->weather->windspeed_unit);
            print_fixed_line(line_buffer, CONTENT_WIDTH);
            
            snprintf(line_buffer, sizeof(line_buffer), "  Vindriktning:  %.0i %s",
              Selected_City->weather->winddirection,
              Selected_City->weather->winddirection_unit);
            print_fixed_line(line_buffer, CONTENT_WIDTH);
            
            snprintf(line_buffer, sizeof(line_buffer), "  Nederbörd:     %.2lf %s",
              Selected_City->weather->precipitation,
              Selected_City->weather->precipitation_unit);
            print_fixed_line(line_buffer, CONTENT_WIDTH);
            
            printf("╚══════════════════════════════════════════╝\n\n");
          }

          /* Get forecast (BREAK THIS OUT INTO OWN CASE!
           * Better yet, own function alongside current weather case) */
          result = city_get_weather(Selected_City, true);
          if (result != 0) {
            printf("Något gick fel med att hämta väderdata! (Kod: %i)\n", result);
          }
          else
          {
            /* Extra careful, make sure there is an initialized Weather struct in Forecast */
            if (sizeof(Selected_City->forecast->weather[0]) == sizeof(Weather))
            {
              

              const int CONTENT_WIDTH = 42;

              printf("\n");
              printf("╔══════════════════════════════════════════╗\n");

              /* Header */
              char header_buffer[128];
              snprintf(header_buffer, sizeof(header_buffer), "  Prognos för: %s", Selected_City->name);
              print_fixed_line(header_buffer, CONTENT_WIDTH);

              int i;
              for (i = 0; i < Selected_City->forecast->count; i++)
              {

                const char* description = get_weather_description(Selected_City->forecast->weather[i].weather_code);
                const char* ascii = get_weather_ascii_art(Selected_City->forecast->weather[i].weather_code);
                printf("╠══════════════════════════════════════════╣\n");

                /* ASCII art */
                char ascii_copy[512];
                strncpy(ascii_copy, ascii, sizeof(ascii_copy) - 1);
                ascii_copy[sizeof(ascii_copy) - 1] = '\0';

                char* line = strtok(ascii_copy, "\n");
                while (line != NULL) {
                  char ascii_line[128];
                  snprintf(ascii_line, sizeof(ascii_line), " %s ", line);
                  print_fixed_line(ascii_line, CONTENT_WIDTH);
                  line = strtok(NULL, "\n");
                }

                /* Empty line */
                print_fixed_line("", CONTENT_WIDTH);

                /* Weather description */
                char desc_buffer[128];
                snprintf(desc_buffer, sizeof(desc_buffer), "  %s", description);
                print_fixed_line(desc_buffer, CONTENT_WIDTH);

                /* Empty line */
                print_fixed_line("", CONTENT_WIDTH);

                /* Weather data */
                char line_buffer[128];


                snprintf(line_buffer, sizeof(line_buffer), "  Tidpunkt:      %s", 
                    epoch_to_iso_string(&Selected_City->forecast->weather[i].timestamp));
                print_fixed_line(line_buffer, CONTENT_WIDTH);

                snprintf(line_buffer, sizeof(line_buffer), "  Temperatur:    %.1lf %s", 
                    Selected_City->forecast->weather[i].temperature,
                    Selected_City->forecast->weather[i].temperature_unit);
                print_fixed_line(line_buffer, CONTENT_WIDTH);

                snprintf(line_buffer, sizeof(line_buffer), "  Vindstyrka:    %.1lf %s",
                    Selected_City->forecast->weather[i].windspeed,
                    Selected_City->forecast->weather[i].windspeed_unit);
                print_fixed_line(line_buffer, CONTENT_WIDTH);

                snprintf(line_buffer, sizeof(line_buffer), "  Vindriktning:  %.0i %s",
                    Selected_City->forecast->weather[i].winddirection,
                    Selected_City->forecast->weather[i].winddirection_unit);
                print_fixed_line(line_buffer, CONTENT_WIDTH);

                snprintf(line_buffer, sizeof(line_buffer), "  Nederbörd:     %.2lf %s",
                    Selected_City->forecast->weather[i].precipitation,
                    Selected_City->forecast->weather[i].precipitation_unit);
                print_fixed_line(line_buffer, CONTENT_WIDTH);

              }
              printf("╚══════════════════════════════════════════╝\n\n");
            } 
            else 
            {
              printf("Ingen väderprogrons hittad för given stad...\n");
            }
          }
        }
        else {
          printf("\nOgiltigt val! Välj en siffra ur listan.\n");
        }
        break;
      }
      
      case 0: /* Avsluta */
        printf("\nTack för att du använder Weatherio! Hejdå!\n");
        running = 0;
        break;
      
      default:
        printf("\nOgiltigt val! Välj 1 eller 2.\n");
        break;
    }
  }

  return 0;
}
