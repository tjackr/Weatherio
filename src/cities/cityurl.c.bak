#include <string.h>
#include <stdio.h>
#include <ctype.h>

char* cityurl (const char* cities, char* url)
{
    int citieslen = strlen(cities);
    int offset = 0;
    char city[32], lat[16], lon[16];
    while (citieslen - offset > 1) /* print list of cities */
    {
        sscanf(cities+offset, "%31[^:]:%15[^:]:%15[^\n]\n", city, lat, lon);
        offset += strlen(city) + strlen(lat) + strlen(lon)+3;
        printf("%s\n", city);
    }

    printf("Välj stad: ");
    char val[32];
    scanf(" %31s", val);
    /* formatera val med stor bokstav */
    val[0] = toupper(val[0]);
    int i;
    for (i = 1; i < (int)strlen(val); i++)
    {
        val[i] = tolower(val[i]);
    }

    offset = 0;
    while (citieslen - offset > 1) /* printa URL baserat på val */
    {
        sscanf(cities+offset, "%31[^:]:%15[^:]:%15[^\n]\n", city, lat, lon);
        offset += strlen(city) + strlen(lat) + strlen(lon)+3;
        if (strcmp(val, city) == 0)
        {
            sprintf(url, "https://api.open-meteo.com/v1/forecast?latitude=%s&longitude=%s&current_weather=true", lat, lon);
            printf("URL: \"%s\"\r\n", url);
        }
    }

    if (url[0] == '\0') /* printa felmeddelande om staden inte finns */
    {
        sprintf(url, "Staden %s finns inte i listan\n", val);
    }

    /* Clear input buffer */
    int c;
    while ((c = getchar()) != '\n' && c != EOF);

    return url;
}
