#include <stdio.h>
#include "includes/jansson/jansson.h"

int main(void) {

  printf("Hello Server!\r\n");

  json_t* json = NULL;

  json_decref(json);

  return 0;
}
