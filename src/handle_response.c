#include <stdio.h>

char* save_response_to_file (const char* res)
{
  FILE * res_json;
  res_json = fopen ("/data/temp/meteo_response.json","w");
  if (res_json == NULL)
  {
    printf("Error writing to file: %s", (char*)res_json);

  }
  fputs (res, res_json);
  fclose (res_json);
  return 0;
}
