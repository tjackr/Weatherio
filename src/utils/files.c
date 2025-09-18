#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Util for loading large files into a*/
char** loadfile(char* filename, int* len)
{
  /* Step size for the chunk building */ 
  const int step_size = 10;

  FILE* f = fopen(filename, "r");
  if (!f)
  {
    fprintf(stderr, "Failed to load file: %s\n", filename);
    return NULL;
  }

  /* Preallocate a chunk of memory that is going to fill the array */
  int chunk_size = step_size;
  char** lines = (char **)malloc(step_size * sizeof(char *));

  int i = 0;
  char buf[1000];
  while (fgets(buf, 1000, f))
  {
    
    int slen = strlen(buf);

    /* Handle if we exceed */
    if (i == chunk_size)
    {
      chunk_size += step_size; /* Increment the chunk size by the step size */
      char** newlines = realloc(lines, 20 *  sizeof(char*));
      if (newlines == NULL)
      {
        printf("Couldn't reallocate new lines\n");
        exit(0);
      }
    }

    /* Allocate space for the string */
    char* str = (char *)malloc((slen + 1) * sizeof(char));
    
    /* Copy the buffer to str */
    strcpy(str, buf);

    /* Attach the string to our ptrptr */
    lines[i] = str;

    i++;
  }

  fclose(f);
  return lines;
}


/* This doesn't really do anything useful */
void save_json_to_file (const char* json, char* filename)
{
  FILE* res_json = fopen (filename,"w");
  if (!res_json)
  {
    printf("Error writing to file: %s", (char*)res_json);

  }
  fputs (json, res_json);
  fclose (res_json);
}

