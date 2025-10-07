#include "weather_codes.h"
#include <stdio.h>

static const WeatherCode weather_codes[] = {
  {0, "Klart väder", 
      "    \\   /    \n"
      "     .-.     \n"
      "  ― (   ) ―  \n"
      "     `-'     \n"
      "    /   \\    "},
  
  {1, "Mestadels klart", 
      "   \\  /      \n"
      " _ /\"\".-.    \n"
      "   \\_(   ).  \n"
      "   /(___(__)  \n"},
  
  {2, "Delvis molnigt",
      "   \\  /      \n"
      " _ /\"\".-.    \n"
      "   \\_(   ).  \n"
      "   /(___(__)  \n"},
  
  {3, "Mulet",
      "             \n"
      "     .--.    \n"
      "  .-(    ).  \n"
      " (___.__)__) \n"},
  
  {45, "Dimma",
       "             \n"
       " _ - _ - _ - \n"
       "  _ - _ - _  \n"
       " _ - _ - _ - \n"},
  
  {48, "Frostdimma",
       "             \n"
       " _ - _ - _ - \n"
       "  _ - _ - _  \n"
       " _ - _ - _ - \n"},
  
  {51, "Lätt duggregn",
       "     .-.     \n"
       "    (   ).   \n"
       "   (___(__)  \n"
       "    ' ' ' '  \n"
       "   ' ' ' '   \n"},
  
  {53, "Måttligt duggregn",
       "     .-.     \n"
       "    (   ).   \n"
       "   (___(__)  \n"
       "    ' ' ' '  \n"
       "   ' ' ' '   \n"},
  
  {55, "Tätt duggregn",
       "     .-.     \n"
       "    (   ).   \n"
       "   (___(__)  \n"
       "    ' ' ' '  \n"
       "   ' ' ' '   \n"},
  
  {61, "Lätt regn",
       "     .-.     \n"
       "    (   ).   \n"
       "   (___(__)  \n"
       "    ʻ ʻ ʻ ʻ  \n"
       "   ʻ ʻ ʻ ʻ   \n"},
  
  {63, "Måttligt regn",
       "     .-.     \n"
       "    (   ).   \n"
       "   (___(__)  \n"
       "  ‚ʻ‚ʻ‚ʻ‚ʻ   \n"
       "  ‚ʻ‚ʻ‚ʻ‚ʻ   \n"},
  
  {65, "Kraftigt regn",
       "     .-.     \n"
       "    (   ).   \n"
       "   (___(__)  \n"
       "  ‚ʻ‚ʻ‚ʻ‚ʻ   \n"
       "  ‚ʻ‚ʻ‚ʻ‚ʻ   \n"},
  
  {71, "Lätt snö",
       "     .-.     \n"
       "    (   ).   \n"
       "   (___(__)  \n"
       "    *  *  *  \n"
       "   *  *  *   \n"},
  
  {73, "Måttlig snö",
       "     .-.     \n"
       "    (   ).   \n"
       "   (___(__)  \n"
       "   * * * *   \n"
       "  * * * *    \n"},
  
  {75, "Kraftig snö",
       "     .-.     \n"
       "    (   ).   \n"
       "   (___(__)  \n"
       "  * * * * *  \n"
       " * * * * *   \n"},
  
  {95, "Åskväder",
       "     .-.     \n"
       "    (   ).   \n"
       "   (___(__)  \n"
       "   ʻ⚡︎ʻ⚡︎ʻ⚡︎ʻ   \n"
       "   ʻ ʻ ʻ ʻ   \n"},
  
  {-1, "Okänt", 
       "             \n"
       "      ?      \n"
       "             \n"}
};

const char* get_weather_description(int _code)
{
  for (size_t i = 0; i < sizeof(weather_codes) / sizeof(WeatherCode) - 1; i++) {
    if (weather_codes[i].code == _code) {
      return weather_codes[i].description;
    }
  }
  return weather_codes[sizeof(weather_codes) / sizeof(WeatherCode) - 1].description;
}

const char* get_weather_ascii_art(int _code)
{
  for (size_t i = 0; i < sizeof(weather_codes) / sizeof(WeatherCode) - 1; i++) {
    if (weather_codes[i].code == _code) {
      return weather_codes[i].ascii_art;
    }
  }
  return weather_codes[sizeof(weather_codes) / sizeof(WeatherCode) - 1].ascii_art;
}
