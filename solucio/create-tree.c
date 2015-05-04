/**
 *
 * Tree creation 
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>

#include "create-tree.h"

/**
 *
 * This function returns the hash value for a given string
 *
 */

static int getHashValue(char *cadena)
{
  unsigned int i, len, seed, sum, hash;

  len = strlen(cadena); 
  sum = 0;
  seed = 131;
  for(i = 0; i < len; i++)
    sum = sum * seed + (int)cadena[i];

  hash = sum % HASHSIZE;

  return hash;
}

/**
 *
 * Allocates memory for the hashTable. The hash table is made up of a vector
 * of linked lists. Each list is initialized with zero elements.
 *
 */

static List *allocHashTable(int size)
{
  int i;
  List *hashTable;

  hashTable = malloc(sizeof(List) * size);
  for(i = 0; i < size; i++)
    initList(&(hashTable[i]));

  return hashTable;
}

/**
 *
 * Deletes the hash table
 *
 */

static void freeHashTable(List *hashTable, int size)
{
  int i;

  for(i = 0; i < size; i++)
    deleteList(&(hashTable[i]));

  free(hashTable);
}

/**
 *
 * Given a filename, this function extracts all the valid words
 * within the file and inserts them in a hash table.
 *
 */

static List *processFile(char *filename)
{
  FILE *fp;
  ListData *listData;
  List *hashTable;

  struct stat st;

  char *bytes, paraula[MAXCHAR], *paraula_copy;
  int i, j, nbytes, is_word, valor_hash, done;

  fp = fopen(filename, "r");
  if (!fp) {
    printf("No s'ha pot obrir el fitxer '%s'\n", filename);
    return NULL;
  }

  stat(filename, &st);
  nbytes = st.st_size;

  bytes = malloc(sizeof(char) * nbytes);
  if (!bytes) {
    printf("No he pogut reservar memoria.\n");
    exit(1);
  }

  fread(bytes, sizeof(char), nbytes, fp);
  fclose(fp);

  hashTable = allocHashTable(HASHSIZE);

  i = 0;
  while (i < nbytes)
  {
    while ((i < nbytes) && (isspace(bytes[i]) || (ispunct(bytes[i]) && (bytes[i] != '\'')))) i++; 

    if (i < nbytes) {
      j = 0;
      is_word = 1;
      done = 0;

      while (!done) {

	if ((isalpha(bytes[i])) || (bytes[i] == '\''))
	  paraula[j] = bytes[i];
	else 
	  is_word = 0;

	j++; i++;
	 
	if ((i == nbytes) || (isspace(bytes[i])) || (ispunct(bytes[i]) && (bytes[i] != '\'')))
	  done = 1;
      }

      if (is_word) {

	paraula[j] = 0;

	paraula_copy = malloc(sizeof(char) * (strlen(paraula)+1));

	for(j = 0; j < strlen(paraula); j++)
	  paraula_copy[j] = tolower(paraula[j]);

	paraula_copy[j] = 0;

	valor_hash = getHashValue(paraula_copy);

	/* Search if the key is in the tree */
	listData = findList(&(hashTable[valor_hash]), paraula_copy); 

	if (listData != NULL) {

	  /* We increment the number of times current item has appeared */
	  listData->numTimes++;

	  free(paraula_copy);

	} else {

	  /* If the key is not in the list, allocate memory for the data and
	   * insert it in the list */

	  listData = malloc(sizeof(ListData));
	  listData->key = paraula_copy;
	  listData->numTimes = 1;

	  insertList(&(hashTable[valor_hash]), listData);
	}
      } /* if is_word */
    } /* if isalpha(bytes[i]) */
  } /* while */

  free(bytes);

  return hashTable;
}

/**
 *
 * Copies the contents of the hash table to the tree structure
 *
 */

static void copyHashTable2Tree(List *hastable, RBTree *tree, int idFile, int numFiles)
{
  Node *t;
  RBdata *data;
  ListItem *current;

  char *paraula;
  int i, j, k, len, numItems;

  for(i = 0; i < HASHSIZE; i++) {

    numItems = hastable[i].numItems;
    current = hastable[i].first;

    for(j = 0; j < numItems; j++) {

      /* Search if the key is in the tree */
      t = findNode(tree, current->data->key);

      if (t != NULL) {

	t->data->numFiles++;
	t->data->numTimes[idFile] = current->data->numTimes;
      } else {

	/* If the key is not in the tree, allocate memory for the data
	 * and insert in the tree */

	data = malloc(sizeof(RBdata));

        len = strlen(current->data->key);
	if (tree->maxStringLen < len)
	  tree->maxStringLen = len;

	paraula = malloc(sizeof(char) * (len + 1));
	strcpy(paraula, current->data->key);

	data->key = paraula;
	data->numFiles = 1;
	data->numTimes = malloc(sizeof(int) * numFiles);

	for(k = 0; k < numFiles; k++)
	  data->numTimes[k] = 0;

	data->numTimes[idFile] = current->data->numTimes;

	insertNode(tree, data);
      }

      current = current->next;
    }
  }
}

/**
 *
 * Main function. It accepts a file as command line argument. This file
 * contains a list with the files to be processed.
 *
 */

RBTree *processDatabase(char *dbfile)
{
  FILE *fp;
  int i, nfiles;
  char *p, line[MAXCHAR], path[MAXCHAR], file[MAXCHAR];

  List *hashTable;
  RBTree *tree;

  fp = fopen(dbfile, "r");
  if (!fp) {
    printf("No s'ha pogut obrir el fitxer '%s'\n", dbfile);
    return NULL; 
  }

  fgets(line, MAXCHAR, fp);
  nfiles = atoi(line);

  if (nfiles < 1) {
    printf("El nombre d'arxius al fitxer '%s' no es correcte.\n", dbfile);
    fclose(fp);
    return NULL;
  }

  /* Extract pathname */

  p = strrchr(dbfile, '/');
  if (p == NULL)
    path[0] = 0;
  else {
    strcpy(path, dbfile);
    path[p-dbfile+1] = 0;
  }

  /* Init tree */

  tree = malloc(sizeof(RBTree));
  initTree(tree);

  tree->sizeDb = nfiles;

  for(i = 0; i < nfiles; i++) {

    /* Read line */

    fgets(line, MAXCHAR, fp);
    line[strlen(line)-1]=0;

    /* Generate file to read, including pathname */

    strcpy(file, path);
    strcat(file, line);

    /* Process file */

    printf("\rProcessant fitxer %d de %d ...", i+1, nfiles); fflush(stdout);
    hashTable = processFile(file);

    if (hashTable) { 
      copyHashTable2Tree(hashTable, tree, i, nfiles);
      freeHashTable(hashTable, HASHSIZE);
    }
  }

  fclose(fp);

  printf("\rTots els fitxers han estat processsats.\n");
  printf("Nombre de nodes: %d\n", tree->numNodes);

  return tree;
}


