#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "create-tree.h"
#include "tree-utils.h"

#define MAXCHAR 100

static int menu() 
{
  char str[5];
  int opcio;

  printf("\n\nMenu\n\n");
  printf(" 1 - Creacio de l'arbre\n");
  printf(" 2 - Emmagatzemar arbre\n");
  printf(" 3 - Carregar arbre\n");
  printf(" 4 - Estadistiques arbre\n");
  printf(" 5 - Sortir\n\n");
  printf("   Escull opcio: ");

  fgets(str, 5, stdin);
  opcio = atoi(str); 

  return opcio;
}

static int menu_histogram()
{
  char str[5];
  int opcio;

  printf("\n\nEscull histograma que vols realitzar:\n");
  printf(" 1 - Histograma sobre un node\n");
  printf(" 2 - Histograma sobre l'arbre\n");
  printf("   Escull opcio: ");

  fgets(str, 5, stdin);
  opcio = atoi(str);

  return opcio;
}

int main(int argc, char **argv)
{
  char filename[MAXCHAR];
  int opcio;

  RBTree *tree;

  if (argc != 1)
    printf("Opcions a la linia de comandes ignorades\n");

  tree = NULL;

  do {
    opcio = menu();
    printf("\n\n");

    switch (opcio) {
      case 1:
	if (tree) {
	  printf("Alliberant arbre actual\n");
	  deleteTree(tree);
	  free(tree);
	}

	printf("Introdueix fitxer amb base de dades: ");
	fgets(filename, MAXCHAR, stdin);

	filename[strlen(filename)-1]=0;

	printf("Processant base de dades...\n");
	tree = processDatabase(filename);
	break;

      case 2:
	if (!tree) {
	  printf("No hi ha cap arbre a memoria\n");
	  break;
	}

	printf("Introdueix el nom de fitxer en el qual es desara l'arbre: ");
	fgets(filename, MAXCHAR, stdin);

	filename[strlen(filename)-1]=0;

	printf("Desant arbre...\n");
	saveTree(tree, filename);
	break;

      case 3:
	if (tree) {
	  printf("Alliberant arbre actual\n");
	  deleteTree(tree);
	  free(tree);
	}

	printf("Introdueix nom del fitxer amb l'arbre: ");
	fgets(filename, MAXCHAR, stdin);

	filename[strlen(filename)-1]=0;

	printf("Carregant arbre...\n");
	tree = loadTree(filename);
	break;

      case 4:

	if (tree) {
	  int subopcio = menu_histogram();

	  switch (subopcio) {
	    case 1:
	      wordHistogram(tree);
	      break;

	    case 2:
	      treeHistogram(tree);
	      break;

	    default:
	      printf("Opcio no valida\n");
	      break;
	  }
	} else 
	  printf("No hi ha arbre carregat a memoria.\n");

	break;

      case 5:
	if (tree) {
	  printf("Alliberant memoria\n");
	  deleteTree(tree);
	  free(tree);
	}
	break;

      default:
	printf("Opcio no valida\n");

    } /* switch */
  }
  while (opcio != 5);

  return 0;
}
