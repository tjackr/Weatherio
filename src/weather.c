/* weather.c */
#include "libs/weather.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>

/* internal table */
static const char cities[] =
"Stockholm:59.3293:18.0686\n"
"Göteborg:57.7089:11.9746\n"
"Malmö:55.6050:13.0038\n"
"Uppsala:59.8586:17.6389\n"
"Västerås:59.6099:16.5448\n"
"Örebro:59.2741:15.2066\n"
"Linköping:58.4109:15.6216\n"
"Helsingborg:56.0465:12.6945\n"
"Jönköping:57.7815:14.1562\n"
"Norrköping:58.5877:16.1924\n"
"Lund:55.7047:13.1910\n"
"Gävle:60.6749:17.1413\n"
"Sundsvall:62.3908:17.3069\n"
"Umeå:63.8258:20.2630\n"
"Luleå:65.5848:22.1567\n"
"Kiruna:67.8558:20.2253\n";

void weather_print_cities(void)
{
    const char *p;
    for (p = cities; *p; ) {
        const char *line_start;
        const char *nl;
        size_t line_len;
        char line[256];
        char name[128], lat[64], lon[64];

        line_start = p;
        nl = strchr(line_start, '\n');
        line_len = nl ? (size_t)(nl - line_start) : strlen(line_start);

        if (line_len >= sizeof(line)) line_len = sizeof(line) - 1;
        memcpy(line, line_start, line_len);
        line[line_len] = '\0';

        if (sscanf(line, "%127[^:]:%63[^:]:%63[^\n]", name, lat, lon) == 3) {
            puts(name);
        }

        if (nl) p = nl + 1; else { p = line_start + line_len; break; }
    }
}

/* ASCII/Latin-1-ish case-insensitive compare (basic) */
static int strcmp_ci(const char *a, const char *b)
{
    unsigned char ca, cb;
    while (*a && *b) {
        ca = (unsigned char)*a++;
        cb = (unsigned char)*b++;
        if (tolower(ca) != tolower(cb))
            return (int)tolower(ca) - (int)tolower(cb);
    }
    return (int)((unsigned char)*a) - (int)((unsigned char)*b);
}

int weather_lookup(const char *city,
                   char *out_lat, size_t out_lat_sz,
                   char *out_lon, size_t out_lon_sz)
{
    const char *p;
    if (!city || !out_lat || !out_lon || out_lat_sz == 0 || out_lon_sz == 0) return 0;

    for (p = cities; *p; ) {
        const char *line_start;
        const char *nl;
        size_t line_len;
        char line[256];
        char name[128], lat[64], lon[64];

        line_start = p;
        nl = strchr(line_start, '\n');
        line_len = nl ? (size_t)(nl - line_start) : strlen(line_start);

        if (line_len >= sizeof(line)) line_len = sizeof(line) - 1;
        memcpy(line, line_start, line_len);
        line[line_len] = '\0';

        if (sscanf(line, "%127[^:]:%63[^:]:%63[^\n]", name, lat, lon) == 3) {
            if (strcmp(name, city) == 0) {
                /* bounded copies with guaranteed NUL */
                if (out_lat_sz > 0) {
                    strncpy(out_lat, lat, out_lat_sz - 1);
                    out_lat[out_lat_sz - 1] = '\0';
                }
                if (out_lon_sz > 0) {
                    strncpy(out_lon, lon, out_lon_sz - 1);
                    out_lon[out_lon_sz - 1] = '\0';
                }
                return 1;
            }
        }

        if (nl) p = nl + 1; else { p = line_start + line_len; break; }
    }
    return 0;
}

int weather_lookup_ci(const char *city,
                      char *out_lat, size_t out_lat_sz,
                      char *out_lon, size_t out_lon_sz)
{
    const char *p;
    if (!city || !out_lat || !out_lon || out_lat_sz == 0 || out_lon_sz == 0) return 0;

    for (p = cities; *p; ) {
        const char *line_start;
        const char *nl;
        size_t line_len;
        char line[256];
        char name[128], lat[64], lon[64];

        line_start = p;
        nl = strchr(line_start, '\n');
        line_len = nl ? (size_t)(nl - line_start) : strlen(line_start);

        if (line_len >= sizeof(line)) line_len = sizeof(line) - 1;
        memcpy(line, line_start, line_len);
        line[line_len] = '\0';

        if (sscanf(line, "%127[^:]:%63[^:]:%63[^\n]", name, lat, lon) == 3) {
            if (strcmp_ci(name, city) == 0) {
                if (out_lat_sz > 0) {
                    strncpy(out_lat, lat, out_lat_sz - 1);
                    out_lat[out_lat_sz - 1] = '\0';
                }
                if (out_lon_sz > 0) {
                    strncpy(out_lon, lon, out_lon_sz - 1);
                    out_lon[out_lon_sz - 1] = '\0';
                }
                return 1;
            }
        }

        if (nl) p = nl + 1; else { p = line_start + line_len; break; }
    }
    return 0;
}

/* C89-safe URL builder: pre-check size, then sprintf */
char *weather_build_url(char *buf, size_t bufsize,
                        const char *lat, const char *lon)
{
    const char *prefix = "https://api.open-meteo.com/v1/forecast?latitude=";
    const char *mid    = "&longitude=";
    const char *tail   = "&current_weather=true";
    size_t need;

    if (!buf || bufsize == 0 || !lat || !lon) return 0;

    need = strlen(prefix) + strlen(lat) + strlen(mid) + strlen(lon) + strlen(tail) + 1;
    if (need > bufsize) return 0;

    /* safe because we checked length */
    sprintf(buf, "%s%s%s%s%s", prefix, lat, mid, lon, tail);
    return buf;
}
