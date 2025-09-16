#include <string.h>
#include <stdio.h>
#include <ctype.h>

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

int yes_or_no(char* question, char answer)
{
  printf("%s\n", question);
  scanf(" %c", &answer);

  if (answer == 'n' || answer == 'N')
  {
    return 0; /* No */
  }
  else if (answer == 'y' || answer == 'Y')
  {
    return 1; /* Yes */
  }
  else {
    return 2; /* Invalid */
  }
}

int line_count(const char* string)
{
  int i;
  int lc = 0; 
  int len = strlen(string);
  for (i = 0; i < len; i++)
  {
    if (string[i] == '\n')
      lc++;
  }
  return lc;
}

void save_json_to_file (const char* json, char* filename)
{
  FILE * res_json;
  res_json = fopen (filename,"w");
  if (res_json == NULL)
  {
    printf("Error writing to file: %s", (char*)res_json);

  }
  fputs (json, res_json);
  fclose (res_json);
}
