/* Function to make a struct out of the cities string  */


#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

typedef struct 
{
  char    name[64];
  char    lat[16];
  char    lon[16];
} city;

void format_name(char* input)
{
  int i;
  for (i = 0; i < (int)strlen(input); i++)
  {
    if (i == 0)
    {
      input[i] = toupper(input[i]);
    } else {
      input[i] = tolower(input[i]);
    }
  }
}

/* We should read this from file using fopen in the future */
const char* citystring = "Stockholm:59.3293:18.0686\n" "Göteborg:57.7089:11.9746\n" "Malmö:55.6050:13.0038\n" "Uppsala:59.8586:17.6389\n" "Västerås:59.6099:16.5448\n" "Örebro:59.2741:15.2066\n" "Linköping:58.4109:15.6216\n" "Helsingborg:56.0465:12.6945\n" "Jönköping:57.7815:14.1562\n" "Norrköping:58.5877:16.1924\n" "Lund:55.7047:13.1910\n" "Gävle:60.6749:17.1413\n" "Sundsvall:62.3908:17.3069\n" "Umeå:63.8258:20.2630\n" "Luleå:65.5848:22.1567\n" "Kiruna:67.8558:20.2253\n"; 

void citystring_parse(const char* citystring, char* input_string, city city_ptr) {
  const char *ptr       = citystring;
  const char *tok_start = citystring;
  char* token;

  while (*ptr != '\0')
  {
    if (*ptr == ':' || *ptr == '\n')
    {
      size_t length = ptr - tok_start;
      memcpy(&token, tok_start, length); /* Copy token data to temp buffer */
      token[length] = '\0'; /* Make buffer safe */
      
      if (strcmp(token, input_string) ==0)
      {
        printf("%s", token);
      }

      /* Set token start to the next token */
      tok_start = ptr + 1; 
    }
    ptr++;
  }
}

int main(void) 
{
  char city_input[64] = "stockholm";
  format_name(city_input);

  /*
  char name[64];
  char lat[16];
  char lon[16];
*/

  city c; 
  
  citystring_parse(citystring, city_input, c);

  return 0;
}
