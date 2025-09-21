#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#include "misc.h"

/* Clear stdin */
void clear_input()
{
  int ch; 
  while ((ch = getchar()) != '\n' && ch != EOF);
}

/* Makes first char uppercase, rest lowercase */
void format_name(char* _input)
{
  int i;
  for (i = 0; i < (int)strlen(_input); i++)
  {
    if (i == 0)
    {
      _input[i] = toupper(_input[i]);
    } else {
      _input[i] = tolower(_input[i]);
    }
  }
}

/* Counts lines in a const char* string */
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

/* C89 approved strdup */
char *strdup(const char *str) {
  size_t size = strlen(str) + 1;
  char *copy = (char*)malloc(size);

  if (copy != NULL) 
    memcpy(copy, str, size);

  return copy;
}
