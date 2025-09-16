#include <string.h>
#include <stdio.h>
#include <ctype.h>

/* Clear stdin */
void clear_input()
{
  int ch; 
  while ((ch = getchar()) != '\n' && ch != EOF);
}


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

int yes_or_no(char *question)
{
  char input[2];
  while (1)
  {
    printf("input 1: %s\n", input);
    printf("%s", question);
    fgets(input, sizeof(input), stdin);
    clear_input();

    if (input[1] == '\n') {
      printf("input 2: %s\n", input);
      /* If input is yes or no return 1 or 0 */
      if (input[0] == 'y' || input[0] == 'Y')
        return 1;
      else if (input[0] == 'n' || input[0] == 'N')
        return 0;

    } else {
      printf("input 3: %s\n", input);
      printf("Ogiltigt svar!\n");
    }
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

/*
const char* read_file(char* filepath)
{
  char *ptr;
  return
}
*/

/* This doesn't really do anything useful */
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
