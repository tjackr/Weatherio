#ifndef WEATHER_H
#define WEATHER_H

#include <stddef.h>

void weather_print_cities(void);

int weather_lookup(const char *city,
                   char *out_lat, size_t out_lat_sz,
                   char *out_lon, size_t out_lon_sz);

int weather_lookup_ci(const char *city,
                      char *out_lat, size_t out_lat_sz,
                      char *out_lon, size_t out_lon_sz);

char *weather_build_url(char *buf, size_t bufsize,
                        const char *lat, const char *lon);

#endif /* WEATHER_H */
