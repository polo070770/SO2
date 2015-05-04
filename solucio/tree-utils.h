/**
 *
 * Tree utils 
 *
 *
 */

#ifndef TREE_UTILS_H
#define TREE_UTILS_H

#include "red-black-tree.h"

extern Node sentinel;

void saveTree(RBTree *tree, char *filename);
RBTree *loadTree(char *filename);
void wordHistogram(RBTree *tree);
void treeHistogram(RBTree *tree);

#endif
