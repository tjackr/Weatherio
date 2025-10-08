#include <stdio.h>
#include <string.h>
#include <time.h>

#include "cli.h"
#include "../utils/misc.h"
#include "../utils/weather_codes.h"
#include "../utils/timeparse.h"

/*============= Internal functions =============*/

int display_menu(void);

int display_interval_menu(void);

void display_current_weather(City *_City);

void display_forecast_table(City *_City, int interval);

static void print_table_cell(const char *_text, int _width);

static void print_table_cell_centered(const char *_text, int _width);

static void print_table_border(const char *left, const char *mid, const char *right, int weather_width);

static void print_table_full_border(const char *left, const char *right, int weather_width);

/*==============================================*/

static int utf8_display_width(const char *_str)
{
  int width = 0;
  const unsigned char *s = (const unsigned char *)_str;

  while (*s)
  {
    if ((*s & 0x80) == 0)
    {
      /* 1-byte ASCII (0xxxxxxx) */
      width++;
      s++;
    }
    else if ((*s & 0xE0) == 0xC0)
    {
      /* 2-byte UTF-8 (110xxxxx 10xxxxxx) */
      width++;
      s += 2;
    }
    else if ((*s & 0xF0) == 0xE0)
    {
      /* 3-byte UTF-8 (1110xxxx 10xxxxxx 10xxxxxx) */
      width++;
      s += 3;
    }
    else if ((*s & 0xF8) == 0xF0)
    {
      /* 4-byte UTF-8 (11110xxx 10xxxxxx 10xxxxxx 10xxxxxx) */
      width++;
      s += 4;
    }
    else
    {
      /* Invalid byte - skip */
      s++;
    }
  }

  return width;
}

/* Function to print a string with fixed width */
/* Add this function after utf8_display_width */
static void print_fixed_line(const char *_text, int _width)
{
  int vis_len = utf8_display_width(_text);

  /* If the text is too long - truncate it */
  if (vis_len > _width)
  {
    const unsigned char *s = (const unsigned char *)_text;
    int printed = 0;

    printf("│");
    while (*s && printed < _width)
    {
      if ((*s & 0x80) == 0)
      {
        /* ASCII */
        putchar(*s);
        s++;
        printed++;
      }
      else if ((*s & 0xE0) == 0xC0)
      {
        /* 2-byte UTF-8 */
        if (printed + 1 <= _width)
        {
          putchar(*s++);
          putchar(*s++);
          printed++;
        }
        else
          break;
      }
      else if ((*s & 0xF0) == 0xE0)
      {
        /* 3-byte UTF-8 */
        if (printed + 1 <= _width)
        {
          putchar(*s++);
          putchar(*s++);
          putchar(*s++);
          printed++;
        }
        else
          break;
      }
      else if ((*s & 0xF8) == 0xF0)
      {
        /* 4-byte UTF-8 */
        if (printed + 1 <= _width)
        {
          putchar(*s++);
          putchar(*s++);
          putchar(*s++);
          putchar(*s++);
          printed++;
        }
        else
          break;
      }
      else
      {
        s++;
      }
    }

    /* Fill the rest with spaces */
    for (int i = printed; i < _width; i++)
    {
      putchar(' ');
    }
    printf("║\n");
  }
  else
  {
    /* If the text fits - print with padding */
    printf("║%s", _text);
    for (int i = vis_len; i < _width; i++)
    {
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
  printf("2. Visa 7-dagars prognos\n");
  printf("0. Avsluta programmet\n");
  printf("Välj alternativ: ");

  scanf("%5i", &choice);
  clear_input();

  return choice;
}

int display_interval_menu(void)
{
  int choice;
  printf("\n=== VÄLJ TIDSINTERVALL ===\n");
  printf("1. Varje timme (168 timmar)\n");
  printf("2. Var 3:e timme (56 timmar)\n");
  printf("3. Var 6:e timme (28 timmar)\n");
  printf("0. Tillbaka till huvudmenyn\n");
  printf("Välj intervall (0-3): ");

  scanf("%5i", &choice);
  clear_input();

  /* Convert menu choice to hour interval */
  switch (choice)
  {
  case 1:
    return 1; /* Every hour */
  case 2:
    return 3; /* Every 3 hours */
  case 3:
    return 6; /* Every 6 hours */
  case 0:
    return 0; /* Back to main menu */
  default:
    printf("\nOgiltigt val! Använder 6-timmars intervall som standard.\n");
    return 6; /* Default to 6 hours */
  }
}

/* Display current weather in a nice box */
void display_current_weather(City *_City)
{
  const char *description = get_weather_description(_City->weather->weather_code);
  const char *ascii = get_weather_ascii_art(_City->weather->weather_code);

  const int CONTENT_WIDTH = 42;

  printf("\n");
  printf("╔══════════════════════════════════════════╗\n");

  /* Header */
  char header_buffer[128];
  snprintf(header_buffer, sizeof(header_buffer), "  Väder för: %s", _City->name);
  print_fixed_line(header_buffer, CONTENT_WIDTH);

  printf("╠══════════════════════════════════════════╣\n");

  /* ASCII art */
  char ascii_copy[512];
  strncpy(ascii_copy, ascii, sizeof(ascii_copy) - 1);
  ascii_copy[sizeof(ascii_copy) - 1] = '\0';

  char *line = strtok(ascii_copy, "\n");
  while (line != NULL)
  {
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
           _City->weather->temperature,
           _City->weather->temperature_unit);
  print_fixed_line(line_buffer, CONTENT_WIDTH);

  snprintf(line_buffer, sizeof(line_buffer), "  Vindstyrka:    %.1lf %s",
           _City->weather->windspeed,
           _City->weather->windspeed_unit);
  print_fixed_line(line_buffer, CONTENT_WIDTH);

  snprintf(line_buffer, sizeof(line_buffer), "  Vindriktning:  %i %s",
           _City->weather->winddirection,
           _City->weather->winddirection_unit);
  print_fixed_line(line_buffer, CONTENT_WIDTH);

  snprintf(line_buffer, sizeof(line_buffer), "  Nederbörd:     %.2lf %s",
           _City->weather->precipitation,
           _City->weather->precipitation_unit);
  print_fixed_line(line_buffer, CONTENT_WIDTH);

  printf("╚══════════════════════════════════════════╝\n\n");
}

/* Print table cell with proper UTF-8 width handling */
static void print_table_cell(const char *_text, int _width)
{
  int vis_len = utf8_display_width(_text);

  /* Print the text */
  printf("%s", _text);

  /* Add padding spaces */
  for (int i = vis_len; i < _width; i++)
  {
    putchar(' ');
  }
}

/* Print table cell centered with proper UTF-8 width handling */
static void print_table_cell_centered(const char *_text, int _width)
{
  int vis_len = utf8_display_width(_text);

  /* Calculate padding */
  int total_padding = _width - vis_len;
  int left_padding = total_padding / 2;
  int right_padding = total_padding - left_padding;

  /* Print left padding */
  for (int i = 0; i < left_padding; i++)
  {
    putchar(' ');
  }

  /* Print the text */
  printf("%s", _text);

  /* Print right padding */
  for (int i = 0; i < right_padding; i++)
  {
    putchar(' ');
  }
}

/* Helper to print solid border line (for header) */
static void print_table_full_border(const char *left, const char *right, int weather_width)
{
  int total = 12 + 7 + 11 + (weather_width + 2) + 12 + 12 + 4; /* all columns + separators */
  printf("%s", left);
  for (int i = 0; i < total; i++)
    printf("═");
  printf("%s\n", right);
}

/* Helper to print table border line with column separators */
static void print_table_border(const char *left, const char *mid, const char *right, int weather_width)
{
  printf("%s", left);
  for (int i = 0; i < 12; i++)
    printf("═"); /* Datum */
  printf("%s", mid);
  for (int i = 0; i < 7; i++)
    printf("═"); /* Tid */
  printf("%s", mid);
  for (int i = 0; i < 10; i++)
    printf("═"); /* Temp */
  printf("%s", mid);
  for (int i = 0; i < weather_width + 2; i++)
    printf("═"); /* Väderlek (dynamic) */
  printf("%s", mid);
  for (int i = 0; i < 12; i++)
    printf("═"); /* Vind */
  printf("%s", mid);
  for (int i = 0; i < 12; i++)
    printf("═"); /* Nederbörd */
  printf("%s\n", right);
}

/* Display 7-day forecast in table format */
void display_forecast_table(City *_City, int interval)
{
  if (_City->forecast == NULL || _City->forecast->weather == NULL)
  {
    printf("Ingen prognosdata tillgänglig!\n");
    return;
  }

  /* First pass: find the longest weather description in the forecast */
  int max_weather_width = 11; /* minimum width */
  for (int i = 0; i < _City->forecast->count; i++)
  {
    Weather *w = &_City->forecast->weather[i];
    struct tm *timeinfo = localtime(&w->timestamp);
    int hour = timeinfo->tm_hour;

    if (hour % 6 == 0)
    {
      const char *weather_desc = get_weather_description(w->weather_code);
      int desc_width = utf8_display_width(weather_desc);
      if (desc_width > max_weather_width)
      {
        max_weather_width = desc_width;
      }
    }
  }
  /* Add buffer for UTF-8 characters that might be miscounted */
  max_weather_width += 2;

  printf("\n");

  /* Top border */
  print_table_full_border("╔", "╗", max_weather_width);

  /* Calculate proper padding for city name with UTF-8 support */
  char header_text[128];
  snprintf(header_text, sizeof(header_text), "7-dagars väder prognos för %s", _City->name);
  int header_vis_width = utf8_display_width(header_text);
  int total_width = 12 + 7 + 11 + (max_weather_width + 2) + 12 + 12 + 4; /* sum of all columns + borders */
  int left_padding = (total_width - header_vis_width) / 2;
  int right_padding = total_width - header_vis_width - left_padding;

  printf("║");
  for (int i = 0; i < left_padding; i++)
    putchar(' ');
  printf("%s", header_text);
  for (int i = 0; i < right_padding; i++)
    putchar(' ');
  printf("║\n");

  /* Header separator */
  print_table_border("╠", "╦", "╣", max_weather_width);

  /* Column headers */
  printf("║ ");
  print_table_cell_centered("Datum", 10);
  printf(" ║ ");
  print_table_cell_centered("Tid", 5);
  printf(" ║ ");
  print_table_cell_centered("Temp", 8);
  printf(" ║ ");
  print_table_cell_centered("Väderlek", max_weather_width);
  printf(" ║ ");
  print_table_cell_centered("Vind", 10);
  printf(" ║ ");
  print_table_cell_centered("Nederbörd", 10);
  printf(" ║\n");

  /* Header bottom */
  print_table_border("╠", "╬", "╣", max_weather_width);

  /* Loop through forecast data */
  int days_shown = 0;
  char last_date[11] = "";

  for (int i = 0; i < _City->forecast->count && days_shown < 7; i++)
  {
    Weather *w = &_City->forecast->weather[i];

    /* Convert timestamp to local time */
    struct tm *timeinfo = localtime(&w->timestamp);

    /* Format date and time */
    char date_str[11];
    char time_str[6];
    strftime(date_str, sizeof(date_str), "%Y-%m-%d", timeinfo);
    strftime(time_str, sizeof(time_str), "%H:%M", timeinfo);

    /* Check if we've moved to a new day */
    if (strcmp(date_str, last_date) != 0)
    {
      if (days_shown > 0)
      {
        print_table_border("╠", "╬", "╣", max_weather_width);
      }
      strncpy(last_date, date_str, sizeof(last_date));
      days_shown++;
    }

    int hour = timeinfo->tm_hour;
    if (hour % interval == 0)
    {
      const char *weather_desc = get_weather_description(w->weather_code);

      /* Build formatted strings for each cell */
      char temp_str[16];
      char wind_str[20];
      char precip_str[20];

      snprintf(temp_str, sizeof(temp_str), "%.1f°C", w->temperature);
      snprintf(wind_str, sizeof(wind_str), "%.1f %s", w->windspeed, w->windspeed_unit);
      snprintf(precip_str, sizeof(precip_str), " %.1f %s", w->precipitation, w->precipitation_unit);

      /* Print row with proper alignment */
      printf("║ ");
      print_table_cell_centered(date_str, 10);
      printf(" ║ ");
      print_table_cell_centered(time_str, 5);
      printf(" ║ ");
      print_table_cell_centered(temp_str, 8);
      printf(" ║ ");
      print_table_cell(weather_desc, max_weather_width); /* Left-aligned for readability */
      printf(" ║ ");
      print_table_cell_centered(wind_str, 10);
      printf(" ║ ");
      print_table_cell_centered(precip_str, 10);
      printf(" ║\n");
    }
  }

  /* Bottom border */
  print_table_border("╚", "╩", "╝", max_weather_width);
  printf("\n");
}

/* Main CLI loop */
int cli_init(Cities *_Cities)
{

  int result;
  printf("\n\n"
         "Varmt välkommen till \n"
         "--------------------\n"
         "▖  ▖    ▗ ▌     ▘  ▌\n"
         "▌▞▖▌█▌▀▌▜▘▛▌█▌▛▘▌▛▌▌\n"
         "▛ ▝▌▙▖█▌▐▖▌▌▙▖▌ ▌▙▌▖\n"
         "--------------------\n\n\n");

  int running = 1;

  while (running)
  {
    int menu_choice = display_menu();

    switch (menu_choice)
    {
    case 1: /* Visa väderdata */
    {
      printf("\nTillgängliga städer:\n");

      City *Selected_City;
      int cities_count = cities_print(_Cities);
      int input_choice = 0;

      printf("\nVälj stad: ");
      scanf("%5i", &input_choice);
      clear_input();

      if (input_choice <= cities_count && input_choice != 0)
      {
        result = city_get_by_index(_Cities, &cities_count, &input_choice, &Selected_City);
        if (result != 0)
        {
          printf("Något gick fel med att hämta tillgängliga städer! (Kod: %i)\n", result);
          break;
        }

        result = city_get_weather(Selected_City, false);
        if (result != 0)
        {
          printf("Något gick fel med att hämta väderdata! (Kod: %i)\n", result);
        }
        else
        {
          display_current_weather(Selected_City);
        }
      }
      else
      {
        printf("\nOgiltigt val! Välj en siffra ur listan.\n");
      }
      break;
    }

    case 2: /* Visa 7-dagars prognos */
    {
      printf("\nTillgängliga städer:\n");

      City *Selected_City;
      int cities_count = cities_print(_Cities);
      int input_choice = 0;

      printf("\nVälj stad för prognos: ");
      scanf("%5i", &input_choice);
      clear_input();

      if (input_choice <= cities_count && input_choice != 0)
      {
        result = city_get_by_index(_Cities, &cities_count, &input_choice, &Selected_City);
        if (result != 0)
        {
          printf("Något gick fel med att hämta tillgängliga städer! (Kod: %i)\n", result);
          break;
        }

        result = city_get_weather(Selected_City, true);
        if (result != 0)
        {
          printf("Något gick fel med att hämta prognosdata! (Kod: %i)\n", result);
        }
        else
        {
          /* Ask user for time interval */
          int interval = display_interval_menu();
          if (interval == 0)
          {
            /* User chose to go back to main menu */
            printf("\nÅtervänder till huvudmenyn...\n");
            break;
          }
          display_forecast_table(Selected_City, interval);
        }
      }
      else
      {
        printf("\nOgiltigt val! Välj en siffra ur listan.\n");
      }
      break;
    }

    case 0: /* Avsluta */
      printf("\nTack för att du använder Weatherio! Hejdå!\n");
      running = 0;
      break;

    default:
      printf("\nOgiltigt val! Välj 1, 2 eller 0.\n");
      break;
    }
  }
  return 0;
}