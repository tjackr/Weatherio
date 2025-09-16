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
