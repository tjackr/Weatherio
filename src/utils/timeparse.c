#include "timeparse.h"

#include <stdio.h>
#include "misc.h"

/*============= Internal functions =============*/

time_t timegm(struct tm* _tm);

/*==============================================*/

#ifndef timegm
/* chatte generated custom timegm for embedded platforms*/
time_t timegm(struct tm* _tm) {
    static const int days_in_month[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
    int year = _tm->tm_year + 1900;
    int month = _tm->tm_mon;
    int day = _tm->tm_mday - 1; /* struct tm days start from 1, Unix epoch from 0 */

    /* Calculate days before current year */
    int days = (year - 1970) * 365 + ((year - 1969) / 4) - ((year - 1901) / 100) + ((year - 1601) / 400);
    /* Add days for previous months this year */
    for (int i = 0; i < month; ++i)
        days += days_in_month[i];
    /* Leap year adjustment */
    if (month > 1 && ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0))
        days += 1;
    /* Add current day */
    days += day;
    /* Convert to seconds */
    return (time_t)((((days * 24 + _tm->tm_hour) * 60 + _tm->tm_min) * 60) + _tm->tm_sec);
}
#endif

/* Helper for parsing iso8601 formatted datetime string to time_t epoch */
time_t parse_iso_datetime(const char* _time_str) 
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

  return timegm(&tm);
}

const char* epoch_to_iso_string(time_t* _epoch)
{
  struct tm* tm = gmtime(_epoch);
  if (!tm) return strdup("Invalid datetime");

  int year = tm->tm_year + 1900;
  int month = tm->tm_mon + 1;
  int day = tm->tm_mday;
  int hour = tm->tm_hour;
  int min = tm->tm_min;

  char iso_string[17];
  if (snprintf(iso_string, 17, "%04d-%02d-%02dT%02d:%02d", year, month, day, hour, min) < 0) {
    return strdup("Invalid datetime");
  }
  return strdup(iso_string);
}
