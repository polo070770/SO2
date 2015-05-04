/**
 *
 * Red-black tree header 
 *
 * See red-black-tree.h for details.
 * 
 *
 */

typedef char * string;
#define TYPEKEY string

#define MAXWORD 50

/**
 *
 * This structure holds the information to be stored at each node. Change this
 * structure according to your needs.  In order to make this library work, you
 * also need to adapt the functions compLT, compEQ and freeRBdata. For the
 * current implementation the "key" member is used to index data within the
 * tree. 
 *
 */

typedef struct RBdata_
{
  // The variable used to index the tree has to be called "key".
  // The type may be any you want (float, char *, etc)
  TYPEKEY key;     

  // This is the additional information that will be stored
  // within the structure.
  int num;  // num of files where it shows
  int * vector; // num of times where it shows on every file

} RBdata;

/**
 *
 * The node structure 
 *
 */

typedef enum { BLACK, RED } nodeColor;

typedef struct Node_ {
    /* For internal use of the structure. Do not change. */
    struct Node_ *left;         /* left child */
    struct Node_ *right;        /* right child */
    struct Node_ *parent;       /* parent */
    nodeColor color;            /* node color (BLACK, RED) */

    /* Data to be stored at each node */
    RBdata *data;                    /* data stored in node */
} Node;

/**
 *
 * The tree structure. It just contains the root node, from
 * which we may go through all the nodes of the binary tree.
 *
 */

typedef struct RBTree_ {
  Node *root;                   /* root of Red-Black tree */
} RBTree;

/*
 * Function headers. Note that not all the functions of
 * red-black-tree.c have been included here.
 */

void initTree(RBTree *tree);
Node *insertNode(RBTree *tree, RBdata *data);
void deleteNode(RBTree *tree, Node *z); 
Node *findNode(RBTree *tree, TYPEKEY key); 
void deleteTree(RBTree *tree);

void printRBTree(RBTree *tree, int numFiles);
void printTree(Node *x, int numFiles);


