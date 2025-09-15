/* main.c */
#include <stdio.h>
#include <string.h>
#include <locale.h>

/*Requirements for JSON library

#ifndef JSON_INLINE
#define JSON_INLINE
#endif
#ifndef INLINE
#define INLINE
#endif
#include <jansson.h>
*/

#include "libs/weather.h"
#include "libs/http.h"



static void rstrip_newline(char *s)
{
    size_t n = strlen(s);
    while (n && (s[n-1] == '\n' || s[n-1] == '\r')) {
        s[--n] = '\0';
    }
}

/* Parse Open-Meteo JSON and print current weather fields
static void parse_weather(const char *json_text)
{
    json_error_t error;
    json_t *root;
    json_t *current;
    json_t *jtemp;
    json_t *jwind;
    json_t *jdir;
    json_t *jtime;

    if (!json_text) return;

    root = json_loads(json_text, 0, &error);
    if (!root) {
        fprintf(stderr, "JSON parse error: %s (line %d, col %d)\n",
                error.text, error.line, error.column);
        return;
    }

    current = json_object_get(root, "current_weather");
    if (!json_is_object(current)) {
        puts("No current_weather in response.");
        json_decref(root);
        return;
    }

    jtemp = json_object_get(current, "temperature");
    jwind = json_object_get(current, "windspeed");
    jdir  = json_object_get(current, "winddirection");
    jtime = json_object_get(current, "time");

    if (json_is_number(jtemp)) {
        printf("Temperature: %.1f °C\n", json_number_value(jtemp));
    } else {
        puts("Temperature: (missing)");
    }

    if (json_is_number(jwind)) {
        printf("Windspeed: %.1f km/h\n", json_number_value(jwind));
    } else {
        puts("Windspeed: (missing)");
    }

    if (json_is_number(jdir)) {
        printf("Wind direction: %.0f°\n", json_number_value(jdir));
    } else {
        puts("Wind direction: (missing)");
    }

    if (json_is_string(jtime)) {
        printf("Time: %s\n", json_string_value(jtime));
    } else {
        puts("Time: (missing)");
    }

    json_decref(root);
}
    */


int main(void)
{
    http h;
    char city[128];
    char lat[64];
    char lon[64];
    char url[256];
    char response[8192];

    /* for å/ä/ö printing; harmless if locale not set */
    setlocale(LC_ALL, "");

    if (http_init(&h) != 0) {
        fprintf(stderr, "Failed to init HTTP\n");
        return 1;
    }

    puts("Welcome!");

    for (;;) {
        puts("\nAvailable cities:");
        weather_print_cities();

        printf("\nEnter city (or 'q' to quit): ");
        if (!fgets(city, sizeof(city), stdin)) break;
        rstrip_newline(city);

        if (city[0] == '\0') {
            puts("Empty input, try again.");
            continue;
        }
        if (strcmp(city, "q") == 0 || strcmp(city, "Q") == 0) {
            puts("Goodbye!");
            break;
        }

        if (!weather_lookup_ci(city, lat, sizeof(lat), lon, sizeof(lon))) {
            printf("City \"%s\" not found, try again.\n", city);
            continue;
        }

        if (!weather_build_url(url, sizeof(url), lat, lon)) {
            puts("Failed to build URL.");
            continue;
        }

        printf("\nFetching:\n%s\n\n", url);

       if (http_get(url, response, sizeof(response)) == 0) {
        
    /* Optional: raw JSON
       printf("Response:\n%s\n", response);
    parse_weather(response); */

} else {
    puts("HTTP request failed.");
}

    }

    http_dispose(&h);
    return 0;
}
