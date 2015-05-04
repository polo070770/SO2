/**
 *
 * Red-black tree implementation. 
 * 
 * Binary search trees work best when they are balanced or the path length from
 * root to any leaf is within some bounds. The red-black tree algorithm is a
 * method for balancing trees. The name derives from the fact that each node is
 * colored red or black, and the color of the node is instrumental in
 * determining the balance of the tree. During insert and delete operations,
 * nodes may be rotated to maintain tree balance. Both average and worst-case
 * search time is O(lg n).
 *
 * This implementation is original from John Morris, University of Auckland, at
 * the following link
 *
 * http://www.cs.auckland.ac.nz/~jmor159/PLDS210/niemann/s_rbt.htm
 *
 * and has been adapted here by Lluis Garrido, 2013.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "red-black-tree.h"

/**
 *
 * Free data element. The user should adapt this function to their needs.  This
 * function is called internally by deleteNode.
 *
 */

static void freeRBdata(RBdata *data)
{

  free(data->key);

  if (data->vector)
    free(data->vector);

  free(data);
}

/**
 *
 * Compares if key1 is less than key2. Should return 1 (true) if condition
 * is satisfied, 0 (false) otherwise.
 *
 */

static int compLT(TYPEKEY key1, TYPEKEY key2)
{

  if (strcmp(key1, key2) < 0)
    return 1;
  else
    return 0;

}

/**
 *
 * Compares if key1 is equal to key2. Should return 1 (true) if condition
 * is satisfied, 0 (false) otherwise.
 *
 */

static int compEQ(TYPEKEY key1, TYPEKEY key2)
{

  if (strcmp(key1, key2) == 0)
    return 1;
  else
    return 0;
}

/**
 * 
 * Please do not change the code below unless you really know what you are
 * doing.
 *
 */

#define NIL &sentinel           /* all leafs are sentinels */
static Node sentinel = { NIL, NIL, 0, BLACK, NULL};

/**
 *
 * Initialize the tree.
 * 
 *
 */

void initTree(RBTree *tree)
{
  tree->root = NIL;
}

/**
 *
 *  Rotate node x to left. Should not be called directly by the user. This
 *  function is used internally by other functions.
 *
 */

static void rotateLeft(RBTree *tree, Node *x) {

  Node *y = x->right;

  /* establish x->right link */
  x->right = y->left;
  if (y->left != NIL) y->left->parent = x;

  /* establish y->parent link */
  if (y != NIL) y->parent = x->parent;
  if (x->parent) {
    if (x == x->parent->left)
      x->parent->left = y;
    else
      x->parent->right = y;
  } else {
    tree->root = y;
  }

  /* link x and y */
  y->left = x;
  if (x != NIL) x->parent = y;
}

/**
 *  
 *  Rotate node x to right. Should not be called directly by the user. This
 *  function is used internally by other functions.
 *
 */

static void rotateRight(RBTree *tree, Node *x) {
  Node *y = x->left;

  /* establish x->left link */
  x->left = y->right;
  if (y->right != NIL) y->right->parent = x;

  /* establish y->parent link */
  if (y != NIL) y->parent = x->parent;
  if (x->parent) {
    if (x == x->parent->right)
      x->parent->right = y;
    else
      x->parent->left = y;
  } else {
    tree->root = y;
  }

  /* link x and y */
  y->right = x;
  if (x != NIL) x->parent = y;
}

/** 
 *
 * Maintain Red-Black tree balance  after inserting node x. Should not be
 * called directly by the user. This function is used internally by other
 * functions.
 *
 */

static void insertFixup(RBTree *tree, Node *x) {
  /* check Red-Black properties */
  while (x != tree->root && x->parent->color == RED) {
    /* we have a violation */
    if (x->parent == x->parent->parent->left) {
      Node *y = x->parent->parent->right;
      if (y->color == RED) {

	/* uncle is RED */
	x->parent->color = BLACK;
	y->color = BLACK;
	x->parent->parent->color = RED;
	x = x->parent->parent;
      } else {

	/* uncle is BLACK */
	if (x == x->parent->right) {
	  /* make x a left child */
	  x = x->parent;
	  rotateLeft(tree,x);
	}

	/* recolor and rotate */
	x->parent->color = BLACK;
	x->parent->parent->color = RED;
	rotateRight(tree, x->parent->parent);
      }
    } else {

      /* mirror image of above code */
      Node *y = x->parent->parent->left;
      if (y->color == RED) {

	/* uncle is RED */
	x->parent->color = BLACK;
	y->color = BLACK;
	x->parent->parent->color = RED;
	x = x->parent->parent;
      } else {

	/* uncle is BLACK */
	if (x == x->parent->left) {
	  x = x->parent;
	  rotateRight(tree, x);
	}
	x->parent->color = BLACK;
	x->parent->parent->color = RED;
	rotateLeft(tree,x->parent->parent);
      }
    }
  }
  tree->root->color = BLACK;
}

/**
 *  
 * Allocate node for data and insert in tree. This function does not perform a
 * copy of data when inserting it in the tree, it rather creates a node and
 * makes this node point to the data. Thus, the contents of data should not be
 * overwritten after calling this function.
 *
 */

Node *insertNode(RBTree *tree, RBdata *data) {
  Node *current, *parent, *x;

  /* Find where node belongs */
  current = tree->root;
  parent = 0;
  while (current != NIL) {
    if (compEQ(data->key, current->data->key)) return (current);
    parent = current;
    current = compLT(data->key, current->data->key) ?
      current->left : current->right;
  }

  /* setup new node */
  if ((x = malloc (sizeof(*x))) == 0) {
    printf ("insufficient memory (insertNode)\n");
    exit(1);
  }

  /* Note that the data is not copied. Just the pointer
     is assigned. This means that the pointer to the 
     data should not be overwritten after calling this
     function. */

  x->data = data;

  /* Copy remaining data */
  x->parent = parent;
  x->left = NIL;
  x->right = NIL;
  x->color = RED;

  /* Insert node in tree */
  if(parent) {
    if(compLT(data->key, parent->data->key))
      parent->left = x;
    else
      parent->right = x;
  } else {
    tree->root = x;
  }

  insertFixup(tree, x);
  return(x);
}

/** 
 *
 * Maintain Red-Black tree balance after deleting node x. Should not be called
 * directly by the user. This function is used internally by other functions.
 *
 */

static void deleteFixup(RBTree *tree, Node *x) {

  while (x != tree->root && x->color == BLACK) {
    if (x == x->parent->left) {
      Node *w = x->parent->right;
      if (w->color == RED) {
	w->color = BLACK;
	x->parent->color = RED;
	rotateLeft (tree, x->parent);
	w = x->parent->right;
      }
      if (w->left->color == BLACK && w->right->color == BLACK) {
	w->color = RED;
	x = x->parent;
      } else {
	if (w->right->color == BLACK) {
	  w->left->color = BLACK;
	  w->color = RED;
	  rotateRight (tree, w);
	  w = x->parent->right;
	}
	w->color = x->parent->color;
	x->parent->color = BLACK;
	w->right->color = BLACK;
	rotateLeft (tree, x->parent);
	x = tree->root;
      }
    } else {
      Node *w = x->parent->left;
      if (w->color == RED) {
	w->color = BLACK;
	x->parent->color = RED;
	rotateRight (tree, x->parent);
	w = x->parent->left;
      }
      if (w->right->color == BLACK && w->left->color == BLACK) {
	w->color = RED;
	x = x->parent;
      } else {
	if (w->left->color == BLACK) {
	  w->right->color = BLACK;
	  w->color = RED;
	  rotateLeft (tree, w);
	  w = x->parent->left;
	}
	w->color = x->parent->color;
	x->parent->color = BLACK;
	w->left->color = BLACK;
	rotateRight (tree, x->parent);
	x = tree->root;
      }
    }
  }
  x->color = BLACK;
}


/**
 *  
 * Deletes node z from the tree. The function re-structures the tree as
 * necessary and de-allocates the node and the data to which the node
 * points to.
 *
 */

void deleteNode(RBTree *tree, Node *z) {
  Node *x, *y;

  if (!z || z == NIL) return;


  if (z->left == NIL || z->right == NIL) {
    /* y has a NIL node as a child */
    y = z;
  } else {
    /* find tree successor with a NIL node as a child */
    y = z->right;
    while (y->left != NIL) y = y->left;
  }

  /* x is y's only child */
  if (y->left != NIL)
    x = y->left;
  else
    x = y->right;

  /* remove y from the parent chain */
  x->parent = y->parent;
  if (y->parent)
    if (y == y->parent->left)
      y->parent->left = x;
    else
      y->parent->right = x;
  else
    tree->root = x;

  /* free data pointed to by the node */
  freeRBdata(z->data);

  if (y != z) 
    z->data = y->data; 

  if (y->color == BLACK)
    deleteFixup (tree, x);

  free(y);
}

/**
 *
 *  Find node containing the specified key. Returns the node.
 *
 */

Node *findNode(RBTree *tree, TYPEKEY key) {

  Node *current = tree->root;

  while(current != NIL)
    if(compEQ(key, current->data->key))
      return (current);
    else
      current = compLT(key, current->data->key) ?
	current->left : current->right;

 return NULL;
}

/**
 *
 *  Function used to delete a tree. Do not call directly. 
 *
 */

static void deleteTreeRecursive(Node *x)
{

  if (x->right != NIL)
    deleteTreeRecursive(x->right);

  if (x->left != NIL)
    deleteTreeRecursive(x->left);

  freeRBdata(x->data);
  free(x);
    
}


/**
 *
 *  Delete a tree. All the nodes and all the data pointed to by
 *  the tree is deleted. 
 *
 */

void deleteTree(RBTree *tree)
{
  if (tree->root != NIL)
    deleteTreeRecursive(tree->root);
}


/**
 * 
 * Store a tree in a file using a recursive function.
 * 
 */

void storeRBTree(RBTree *tree, char * path, int numNodes, int numFiles, int max_word)
{

  FILE * fp;

  fp = fopen(path, "w");
  if (!fp){
    printf("No s'ha pogut crear el fitxer!\n");
    exit(EXIT_FAILURE);
  }

  fwrite(&numNodes, sizeof(int), 1, fp);
  fwrite(&numFiles, sizeof(int), 1, fp);
  fwrite(&max_word, sizeof(int), 1, fp);

  if (tree->root != NIL)
    storeTree(tree->root, fp, numFiles);

  fclose(fp);
}

/**
 *
 * Function used to store a tree in a file.
 *
 */

void storeTree(Node *x, FILE * fp, int numFiles)
{
  
  if (x->right != NIL)
    storeTree(x->right, fp, numFiles);

  if (x->left != NIL)
    storeTree(x->left, fp, numFiles);
  
  fwrite(x->data->key, sizeof(char), MAXWORD, fp);
  fwrite(x->data->vector, sizeof(int), numFiles, fp);
}

/**
 *
 * Count how many words with n letters appear in the files using a recursive dfs function.
 *
 */

void probabilitatLletres(RBTree *tree, int * prob){
  
  if (tree->root != NIL)
    dfs(tree->root, prob);

}

/**
 *
 * DFS function used to count words with n letters.
 *
 */

void dfs(Node *x, int * prob){

  if (x->right != NIL)
    dfs(x->right, prob);

  if (x->left != NIL)
    dfs(x->left, prob);

  int n_lletres = strlen(x->data->key);

  prob[n_lletres-1] += 1;

}