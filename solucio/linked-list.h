/**
 *
 * Linked-list header 
 * 
 * Include this file in order to be able to call the 
 * functions available in linked-list.c. We include
 * here only those information we want to make visible
 * to other files.
 *
 * Lluis Garrido, 2013.
 *
 */

#ifndef LKLIST_H
#define LKLIST_H

#define TYPEKEY char * 

/**
 *
 * This structure holds the information to be stored at each list item.  Change
 * this structure according to your needs.  In order to make this library work,
 * you also need to adapt the functions compEQ and freeListData. For the
 * current implementation the "key" member is used search within the list. 
 *
 */

typedef struct ListData_ {
    TYPEKEY key;
    int numTimes;
} ListData;


/**
 * 
 * The item structure
 *
 */

typedef struct ListItem_ {
  ListData *data;
  struct ListItem_ *next;
} ListItem;

/**
 * 
 * The list structure
 *
 */

typedef struct List_ {
  int numItems;
  ListItem *first;
} List;

/**
 *
 * Function heders we want to make visible so that they
 * can be called from any other file.
 *
 */

void initList(List *l);
void insertList(List *l, ListData *data);
ListData *findList(List *l, TYPEKEY key);
void deleteFirstList(List *l);
void deleteList(List *l);
void dumpList(List *l);

#endif
