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
int line_count(const char* _string)
{
  int i;
  int lc = 0; 
  int len = strlen(_string);
  for (i = 0; i < len; i++)
  {
    if (_string[i] == '\n')
      lc++;
  }
  return lc;
}

/* C89 approved strdup */
char *strdup(const char *_str) {
  size_t size = strlen(_str) + 1;
  char *copy = (char*)malloc(size);

  if (copy != NULL) 
    memcpy(copy, _str, size);

  return copy;
}

/* Concatenates two strings using malloc, strcpy and strcat
 * (goes without saying but needs to be freed by caller) */
char* stringcat(const char* _a, const char* _b)
{
  size_t len = strlen(_a) + strlen(_b) + 1;
  char* ab = (char*)malloc(sizeof(char) * len);
  strcpy(ab, _a);
  strcat(ab, _b);

  return ab;
}
