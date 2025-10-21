#include <stdio.h>
#include "libs/utils/linkedlist.h"

int main(void) {

  int result = 0;
  char* test_string = "Hello this is a test string";
  int foo = 69;

  Linked_List Test_List = {0};
  Linked_Item* Item_One;
  Linked_Item* Item_Two;
  Linked_Item* Item_Three;

  result = linked_list_add(&Test_List, &Item_One, test_string);
  result = linked_list_add(&Test_List, &Item_Two, &foo);
  if (result != 0)
    return result;

  printf("Item_One->Item: %s\n", (char*)Item_One->item);
  printf("Item_two->Item: %i\n", *(int*)Item_Two->item);

  result = linked_list_add(&Test_List, &Item_Three, &foo);
  linked_list_remove(&Test_List, Item_Three);


  printf("Item_Three->Item: %i\n", *(int*)Item_Two->item);

  linked_list_dispose(&Test_List);

  

  return result;
}
