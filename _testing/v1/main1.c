#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <curl/curl.h>

char** cityurl ()
{
    const char* cities = "Stockholm:59.3293:18.0686\n" "Göteborg:57.7089:11.9746\n" "Malmö:55.6050:13.0038\n" "Uppsala:59.8586:17.6389\n" "Västerås:59.6099:16.5448\n" "Örebro:59.2741:15.2066\n" "Linköping:58.4109:15.6216\n" "Helsingborg:56.0465:12.6945\n" "Jönköping:57.7815:14.1562\n" "Norrköping:58.5877:16.1924\n" "Lund:55.7047:13.1910\n" "Gävle:60.6749:17.1413\n" "Sundsvall:62.3908:17.3069\n" "Umeå:63.8258:20.2630\n" "Luleå:65.5848:22.1567\n" "Kiruna:67.8558:20.2253\n"; 

    int citieslen = strlen(cities);
    int offset = 0;
    char city[20], lat[20], lon[20];
    while (citieslen - offset > 1) //print list of cities
    {
        sscanf(cities+offset, "%19[^:]:%19[^:]:%19[^\n]\n", city, lat, lon);
        offset += strlen(city) + strlen(lat) + strlen(lon)+3;
        printf("%s\n", city);
    }

    printf("Välj stad: ");
    char val[20];
    scanf("%19s", val);
    //formatera val med stor bokstav
    val[0] = toupper(val[0]);
    for (int i = 1; i < strlen(val); i++)
    {
        val[i] = tolower(val[i]);
    }

    offset = 0;
    char url[150] = "";
    while (citieslen - offset > 1) //printa URL baserat på val
    {
        sscanf(cities+offset, "%19[^:]:%19[^:]:%19[^\n]\n", city, lat, lon);
        offset += strlen(city) + strlen(lat) + strlen(lon)+3;
        if (strcmp(val, city) == 0)
        {
            sprintf(url, "https://api.open-meteo.com/v1/forecast?latitude=%s&longitude=%s&current_weather=true", lat, lon);
            printf("URL: \"%s\"\r\n", url);
            return url;
        }
    }
    
    if (url[0] == '\0') //printa felmeddelande om staden inte finns
    {
        printf("Staden %s finns inte i listan\n", val);
    }
}

char* get_response(char *url)
{
  CURL *curl;
  CURLcode res;

  curl = curl_easy_init();

  curl_easy_setopt(curl, CURLOPT_URL, url);
  printf("Getting response...\n");
  res = curl_easy_perform(curl);

  if (res != CURLE_OK)
    printf("curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
  
  return res;

}

int main()
{
    
    printf("Välkommen!\n");

    char again;
    do
    {        

      printf("Vill du söka en annan stad? (y/n): ");
      scanf("%c", &again);

      if (again == 'n' || again == 'N')
      {
        break;
      }
      
      char[2048] weather_json = cityurl();

    } while (again != 'n' || again != 'N' || again != 'Y' || again != 'y' );

    return 0;
}
