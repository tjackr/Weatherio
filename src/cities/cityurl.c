#include <stdio.h>

#include "cities.h"

void build_meteourl (char* url, city city)
{
  sprintf(url, "https://api.open-meteo.com/v1/forecast?latitude=%f&longitude=%f&current_weather=true", city.lat, city.lon);
}

