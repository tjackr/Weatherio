#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char** loadfile(char* filename, int* len)
{
  FILE* f = fopen(filename, "r");
  if (!f)
  {
    fprintf(stderr, "Failed to load file: %s\n", filename);
    return NULL;
  }

  /* Preallocate a chunk of memory that is going to fill the array */
  char** lines = (char **)malloc(100 * sizeof(char *));

  char buf[1000];
  int i = 0;
  while (fgets(buf, 1000, f))
  {
    int slen = strlen(buf);

    /* Allocate space for the string */
    char* str = (char *)malloc((slen + 1) * sizeof(char));
    
    strcpy(str, buf);

    lines[i] = str;

    i++;
  }

  fclose(f);
  return lines;
}
