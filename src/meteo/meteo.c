#include <stdio.h>

#include "meteo.h"
#include "../http/http.h"

#include "../utils/files.h"

#include "../includes/cJSON.h"

int meteo_init(Meteo* _meteo, const char* _api_url)
{
	memset(_meteo, 0, sizeof(Meteo));

	_meteo->api_url = _api_url;
	
	return 0;
}

int meteo_build_url(Meteo* _meteo)
{
  /* ---Unfinished
   * We should build the meteo url dynamically with a set of parameters to get specific data
   * User should set options in a seperate function beforehand, either into the meteo struct directly or into an options struct 
   *
   Example parameters:
   hourly=
   daily=
   current=
   current_weather=(true || fase)
   temperature_unit=(fahrenheit || celsius)
   * */
  
  char* finished_url;
  char* base_url = "https://api.open-meteo.com/v1/forecast"; 
  finished_url = base_url;

  _meteo->api_url = finished_url;

	/* sprintf(url, sizeof(url), "%s/v1/forecast?latitude=%2.2f&longitude=%2.2f&hourly=temperature_2m", _meteo->api_url, _lat, _lon); */
  return 0;
}

int meteo_get_temperature(Meteo* _meteo, float _lat, float _lon, char* _name/* , float* _temperature */)
{
	HTTP http;
	if (http_init(&http) != 0)
	{
		printf("Failed to initialize HTTP\n");
		return -1;
	}

	char url[512];

	sprintf(url, "%s?latitude=%f&longitude=%f&current_weather=true", _meteo->api_url, _lat, _lon);
	int result = curl_get_response(&http, url);
	if (result != 0)
	{
		printf("HTTP GET request failed. Errorcode: %i\n", result);
		http_dispose(&http);
		return -2;
	}


  char* basepath = "./data/cache/";
char filepath[512];
strcpy(filepath, basepath);
strcat(filepath, _name);
strcat(filepath, ".json");
printf("Filepath: %s\n", filepath);

 /* char* filepath = malloc (strlen(basepath) + strlen(_name) + strlen(".json") + 1);
  if (filepath == NULL) { printf("Malloc error"); exit (1); }

      (strcpy (basepath, filepath));
      strcpy(_name, filepath[strlen(basepath)]);*/

  /* Here we should call our json parser to get current temperature */
	/* *(_temperature) = 0; */
	char* http_data = http.addr;

	json_handler (http_data, filepath, (char*)_name);

	cJSON* cjson;
    FILE *fp = fopen(filepath, "r");
    if (fp == NULL) {
        printf("Error: Unable to open the file.\n");
        return 1;
    }
    const char* buffer = load_file_as_string1(filepath);

    /*read the file contents into a string

    char buffer[1024];
    int len = fread(buffer, 1, sizeof(buffer), fp);
    fclose(fp);*/
  printf(buffer);

    /*parse the JSON data*/
    cjson = cJSON_Parse(buffer);
    if (cjson == NULL) {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL) {
            printf("Error: %s\n", error_ptr);
        }
        cJSON_Delete(cjson);
        return 1;
        
    }
    cJSON *current_weather = cJSON_GetObjectItemCaseSensitive(cjson, "current_weather");
    if (cJSON_IsString(current_weather) && (current_weather->valuestring != NULL)) {
        printf("Current Weather: %s\n", current_weather->valuestring);
    }
  

	/*printf(cjson->valuestring);

	cJSON* temperature = cJSON_GetObjectItem(current_weather, "temperature");
	printf("Current temperature in %s: %.2f °C\n", _name, temperature->valuedouble);*/



	return 0;
}



void meteo_dispose(Meteo* _m)
{
  
  _m++;
}
