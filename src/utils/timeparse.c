#include "timeparse.h"

#include <stdio.h>

/* Helper for parsing the meteo formatted datetime string */
time_t parse_time_string(const char* _time_str) 
{
  struct tm tm = {0};
  int year, month, day, hour, min;

  if (sscanf(_time_str, "%4d-%2d-%2dT%2d:%2d", &year, &month, &day, &hour, &min) != 5) {
    return (time_t)-1;  
  }

  tm.tm_year = year - 1900;  /* struct tm years since 1900 */
  tm.tm_mon = month - 1;     /* struct tm months are zero-based */
  tm.tm_mday = day;
  tm.tm_hour = hour;
  tm.tm_min = min;
  tm.tm_isdst = -1;  /* Not considering daylight saving time */

/*
  time_t local_time = mktime(&tm);
  struct tm* gm_tm = gmtime(&local_time);
  time_t gm_time = mktime(gm_tm);
*/

  return mktime(&tm);
}

