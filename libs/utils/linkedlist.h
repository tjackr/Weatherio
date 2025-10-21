#ifndef __LINKEDLIST_H__
#define __LINKEDLIST_H__

typedef struct Linked_Item
{
  struct Linked_Item* prev;
  struct Linked_Item* next;

  void* item; /* Item could be anything */
  
} Linked_Item;

typedef struct
{
  Linked_Item* head;
  Linked_Item* tail;

} Linked_List;


//void linked_list_init(Linked_List* _Linked_List); /* Might not be needed? */

int linked_list_add(Linked_List* _Linked_List, Linked_Item** _Linked_Item_Ptr, void* _item);

void linked_list_remove(Linked_List* _Linked_List, Linked_Item* _Linked_Item);

void linked_list_dispose(Linked_List* _Linked_List);

#endif
