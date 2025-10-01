#ifndef __WEATHER_CODES_H__
#define __WEATHER_CODES_H__

typedef struct {
    int code;
    const char* description;
    const char* ascii_art;
} WeatherCode;

const char* get_weather_description(int code);
const char* get_weather_ascii_art(int code);

#endif