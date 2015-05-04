/**
 *
 * Tree utilities
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "tree-utils.h"

/**
 *  Saves recursively the contents of each node. This function is called by
 *  saveTree. Do not call directly.
 *
 */

static void saveTreeRecursive(Node *x, int sizeDb, FILE *fp)
{
  int len;

  len = strlen(x->data->key);

  fwrite(&len, sizeof(int), 1, fp);
  fwrite(x->data->key, sizeof(char), len, fp);
  fwrite(&(x->data->numFiles), sizeof(int), 1, fp);
  fwrite(x->data->numTimes, sizeof(int), sizeDb, fp); 

  if (x->right != NIL)
    saveTreeRecursive(x->right, sizeDb, fp);

  if (x->left != NIL)
    saveTreeRecursive(x->left, sizeDb, fp);
}

/**
 *
 * Save a tree in a file. Only saves contents of each node, does not save the
 * tree structure itself.
 *
 */

void saveTree(RBTree *tree, char *filename)
{
  FILE *fp;
  
  fp = fopen(filename, "w");
  if (!fp) {
    printf("No he pogut obrir el fitxer.\n");
    return;
  }

  fwrite(&(tree->numNodes), sizeof(int), 1, fp);
  fwrite(&(tree->sizeDb), sizeof(int), 1, fp);
  fwrite(&(tree->maxStringLen), sizeof(int), 1, fp);

  saveTreeRecursive(tree->root, tree->sizeDb, fp);

  fclose(fp);
}

/**
 *
 * Loads a tree in memory. The file only contains the information stored in
 * each tree, it does not contain the tree structure. The tree structure is
 * created by inserting the nodes within the tree structure.
 *
 */

RBTree *loadTree(char *filename)
{
  int i, j, len, numNodes;

  FILE *fp;

  RBTree *tree;
  RBdata *data;

  fp = fopen(filename, "r");
  if (!fp) {
    printf("No he pogut obrir el fitxer.\n");
    return NULL;
  }

  tree = malloc(sizeof(RBTree));
  initTree(tree);

  fread(&numNodes, 1, sizeof(int), fp);
  fread(&(tree->sizeDb), 1, sizeof(int), fp);
  fread(&(tree->maxStringLen), 1, sizeof(int), fp);

  printf("Nombre de nodes de l'arbre: %d\n", numNodes);
  printf("Nombre de fitxers a la base de dades: %d\n", tree->sizeDb);

  for(i = 0; i < numNodes; i++)
  {
    data = malloc(sizeof(RBdata));

    j = fread(&len, sizeof(int), 1, fp);
    if (j != 1) {
      printf("ERROR: file does not contain all nodes\n");
      printf("Stopped reading node %d\n", i);
      break; 
    }
    data->key = malloc(sizeof(char) * (len + 1));

    fread(data->key, sizeof(char), len, fp);
    data->key[len] = 0;

    fread(&(data->numFiles), sizeof(int), 1, fp);

    data->numTimes = malloc(sizeof(int) * tree->sizeDb);
    fread(data->numTimes, sizeof(int), tree->sizeDb, fp);

    insertNode(tree, data);
  }

  fclose(fp);

  return tree;
}

/**
 *
 * Used to compute the distribution of words within the tree. This
 * function is called by wordHistogram.
 *
 */

static void treeHistogramRecursive(Node *x, int nFiles, double *histo)
{
  int len;

  len = strlen(x->data->key);
  histo[len] += 1.0;

  if (x->right != NIL)
    treeHistogramRecursive(x->right, nFiles, histo);

  if (x->left != NIL)
    treeHistogramRecursive(x->left, nFiles, histo);
}

/**
 *
 * Plots a histogram using gnuplot
 *
 */

static void drawGnuplot(double *histo, int len)
{
  int i;
  char str[2];
  FILE *fp;

  fp = fopen("/tmp/practica2.data", "w");
  for(i = 0; i < len; i++)
    fprintf(fp, "%d %f\n", i, histo[i]);
  fclose(fp);

  fp = popen("gnuplot", "w");
  fprintf(fp, "set key off\n");
  fprintf(fp, "plot \"/tmp/practica2.data\" with lines\n");
  fflush(fp);
  sleep(2);

  printf("Polsa ENTER per continuar...\n");
  fgets(str, 2, stdin);

  fclose(fp);

}

/**
 * 
 * Computes the histogram of the tree and uses gnuplot
 * to plot the function.
 *
 */

void treeHistogram(RBTree *tree)
{
  int i, len;
  double *histo, sum;

  len = tree->maxStringLen + 1;
  histo = malloc(sizeof(double) * len);

  for(i = 0; i < len; i++)
    histo[i] = 0.0;

  treeHistogramRecursive(tree->root, tree->sizeDb, histo);

  sum = 0.0;
  for(i = 0; i < len; i++)
    sum += histo[i];

  for(i = 0; i < len; i++)
    histo[i] /= sum;

  drawGnuplot(histo, len);

  free(histo);
}

/**
 *
 * Count how many words are in each file
 *
 */

static void wordHistogramCountWordsRecursive(Node *x, int nFiles, int *histo)
{
  int i;

  for(i = 0; i < nFiles; i++)
    histo[i] += x->data->numTimes[i];

  if (x->right != NIL)
    wordHistogramCountWordsRecursive(x->right, nFiles, histo);

  if (x->left != NIL)
    wordHistogramCountWordsRecursive(x->left, nFiles, histo);
}

/**
 * 
 * This function is called by the qsort function. It is used to compare two
 * elements of the vector to order.
 *
 */

static int compare_doubles(const void *p1, const void *p2)
{
  double *num1, *num2;

  num1 = (double *) p1;
  num2 = (double *) p2;

  if (*num1 > *num2)
    return -1;
  if (*num1 < *num2)
    return 1;

  return 0;
}


/**
 * 
 * Computes the histogram of word and uses gnuplot
 * to plot the function
 *
 */

void wordHistogram(RBTree *tree)
{
  int i, len, *words;
  char str[100];
  double *histo, sum;

  Node *t;
  RBdata *data;

  printf("Introdueix la paraula: ");
  fgets(str, 100, stdin);
  str[strlen(str)-1]=0;

  if (strlen(str) == 0) {
    printf("No s'ha introduit cap paraula\n");
    return;
  }

  t = findNode(tree, str);

  if (!t) {
    printf("La paraula no s'ha trobat a l'arbre\n");
    return;
  }

  data = t->data;

  len = tree->sizeDb;
  histo = malloc(sizeof(double) * len);

  words = malloc(sizeof(int) *len);
  for(i = 0; i < len; i++)
    words[i] = 0;

  wordHistogramCountWordsRecursive(tree->root, len, words);

  sum = 0.0;
  for(i = 0; i < len; i++) {
     histo[i] = data->numTimes[i] / (double) words[i];
     sum += histo[i];
  }

  for(i = 0; i < len; i++) 
    histo[i] /= sum;

  qsort(histo, len, sizeof(double), compare_doubles);
     
  drawGnuplot(histo, len);

  free(histo);
  free(words);
}


