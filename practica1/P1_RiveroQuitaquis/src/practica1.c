/**
 *
 * Practica 1 of Operative Systems.
 *
 * Dennis Quitaquis
 * Daniel Rivero
 *
 */

#include <stdio.h> /*fgets() functions*/
#include <stdlib.h> /*For the malloc function*/
#include <string.h> /* isspace(), memset(), strlen(), functions*/
#include <ctype.h>
#include "linked-list.h"
#include "red-black-tree.h"

#define MAXLINEA 200
#define SIZE 100

int funcHash(char *cadena);
void insertHashTable(char * subcadena, List * tablaHash);
void insertTreeRB(char *subcadena, int numTimes, RBTree * tree, int numFile, int numFiles);
void doLine(char *linea, List * tablaHash, char * subcadena);
void insertarArbol(RBTree * tree, List * tablaHash, int numFile, int numFiles);

int main(int argc, char **argv){

	RBTree tree;

	FILE *pathFile;
	FILE *textFile;
	
	char * linea_path;
	char * linea_file;
	char * subcadena;
	char * path;

	int i, k, numFile, numFiles;

	linea_path = malloc(sizeof(char) * MAXLINEA);
	if (linea_path == NULL)
  	{
    	printf("No he pogut reservar la memòria\n");
		exit(1);
  	}

  	linea_file = malloc(sizeof(char) * MAXLINEA);
	if (linea_file == NULL)
  	{
    	printf("No he pogut reservar la memòria\n");
		exit(1);
  	}

  	subcadena = malloc(sizeof(char) * MAXWORD);
	if (subcadena == NULL)
  	{
    	printf("No he pogut reservar la memòria\n");
		exit(1);
  	}

  	path = malloc(sizeof(char) * MAXLINEA);
	if (path == NULL){
	    	printf("No he pogut reservar la memòria\n");
			exit(1);
	}

  	/* Initialize the Tree */
  	initTree(&tree);

  	/* Get the path of the file to read */
	pathFile = fopen(argv[1], "r");

	/* Get the first line of the file of paths.*/
	fgets(linea_path, MAXLINEA, pathFile);
	numFiles = atoi(linea_path);

	numFile = 0;

	/*
		In this bucle we are going to read all the path files.
	*/
	while(fgets(linea_path, MAXLINEA, pathFile) != NULL && numFile < numFiles){

	  	/* Reinitialize the var path */
		memset(path, 0, MAXLINEA);

		k = 0;
		while(linea_path[k] != 10){
			/* Here we copy the path file line without the last character LF (new line) code ASCII */
			path[k] = linea_path[k];
			k ++;
		}

		printf("Abriendo el archivo numero %d\n", numFile + 1);
		
		textFile = fopen(path, "r");

		/* Initialize the table hash */
		List *tablaHash;
		tablaHash = malloc(sizeof(List) * SIZE);
		if (tablaHash == NULL){
			printf("No he pogut reservar la memòria\n");
			exit(1);
		}

		/* Every table hash position we initialize it with the linked list */
		for(i = 0; i < SIZE; i++){
			initList(&tablaHash[i]);
		}

		/*
			In this bucle of every path file, we are going to treat every
			line of the file.
		*/
		while(fgets(linea_file, MAXLINEA, textFile)){
			doLine(linea_file, tablaHash, subcadena);
			memset(linea_file, 0, MAXLINEA);
		}

		/* We realease memory */
		fclose(textFile);
		insertarArbol(&tree, tablaHash, numFile, numFiles);
		
		for(i = 0; i < SIZE; i++){
			deleteList(&tablaHash[i]);
		}
		free(tablaHash);

		numFile ++;
		
	}
	
	/* Print the RBD Tree*/
	printRBTree(&tree, numFiles);
	
	/* We realease memory */
	fclose(pathFile);
	free(linea_file);
	free(linea_path);
	free(path);
	free(subcadena);
	
	deleteTree(&tree);
	
	return 0;
}

/**
 *
 * This func calculates the hash valor of a 
 * string.
 *
 */

int funcHash(char *subcadena){

	int i, len, seed, hash;
	unsigned int sum;
	
	len = strlen(subcadena) - 1;

	sum = 0;
	seed = 131;
	for(i = 0; i < len; i++)
		sum = sum * seed + (int) subcadena[i];
	
	hash = sum % SIZE;

	return hash;
}

/**
 *
 * This func insert a string at the RBD Tree
 *
 */

void insertTreeRB(char *subcadena, int numTimes, RBTree * tree, int numFile, int numFiles){
	
	int i;
	Node *node;
	RBdata *data;

	/* Search if the key is in the tree */
	node = findNode(tree, subcadena);

	if (node != NULL) {

		/*
			If the key is in the tree,
			we increase it the number of times that shows up
			and in what file it showed up
		 */
		data = node->data;

		data->num += numTimes;
		data->vector[numFile] ++;

	}else{

		/* 
			If the key is not in the tree, allocate memory for the data
       		and insert in the tree
       	 */

		data = malloc(sizeof(RBdata));

		data->key = malloc(sizeof(char)*MAXWORD);
		memset(data->key, 0, MAXWORD);

		for (i = 0; i < MAXWORD; i++){
			data->key[i] = subcadena[i];
		}
		data->num = 1;
		data->vector = malloc(sizeof(int)*numFiles);

		for (i = 0; i < numFiles; i++){
			data->vector[i] = 0;
		}

		data->vector[numFile] = numTimes;

		insertNode(tree, data);
	}
}

/**
 *
 * This func insert a string at the hash table
 *
 */

void insertHashTable(char * subcadena, List * tablaHash){

	int valor_hash;

	ListData *listData;

	valor_hash = funcHash(subcadena);

	/* Search if the key is in the tree */
	listData = findList(&tablaHash[valor_hash], subcadena);

	if(listData != NULL){
		
		/* We increase it the number of times that shows up */
		listData->numTimes++;

	}else{

		/*	
			If the key is not in the list, allocate memory for the data and
			insert it in the list
       	 */
		listData = malloc(sizeof(ListData));
		listData->key = malloc(sizeof(TYPEKEY) * MAXWORD);
		
		strcpy(listData->key, subcadena);

		listData->numTimes = 1;
	
		insertList(&tablaHash[valor_hash], listData);

	}

	//dumpList(&tablaHash[valor_hash]);
}

/**
 *
 * This func gets the hash table and for every word in it,
 * it inserts at the RBD Tree.
 *
 */

void insertarArbol(RBTree * tree, List * tablaHash, int numFile, int numFiles){
  
	int i;
	struct ListItem_ * centinela;

	char * cadena = malloc(sizeof(char)*MAXWORD);
	memset(cadena, 0, MAXWORD);
	
	/* Bucle to go over the hash table */
	for(i = 0; i < SIZE; i++){
	  

		centinela = tablaHash[i].first;
	
		while(centinela){
			strcpy(cadena, centinela->data->key);
			
			/**for(j = 0; j < centinela->data->numTimes; j++){
				  insertTreeRB(cadena, numTimes, tree, numFile, numFiles);
			}*/

			/*Anyadimos la palabra las n veces que aparece, si esta no esta la primera vez
			se incializara a uno, pero despues se ira anyadiendo la cantidad de veces que 
			aparece al fichero*/

			insertTreeRB(cadena, centinela->data->numTimes, tree, numFile, numFiles);  
			
			memset(cadena, 0, MAXWORD);
 			centinela = centinela->next;
		}
	}

	free(cadena); 
}

/**
 *
 * This func treats a line to separate it in words and
 * furthermore insert them at the table hash.
 *
 */

void doLine(char *linea, List * tablaHash, char * subcadena){

	int len, i;
	int npalabras = 0;
	
	/* Reinitialize the var subcadena */
  	memset(subcadena, 0, MAXWORD);

  	len = strlen(linea);
  	int j = 0;
  	int isOdd = 0;

  	/* If the line to be treated has characters */
  	if(len > 1){

  		for (i = 0; i < len; i++){

  			//printf(" %c:%d ", linea[i], linea[i]);

  			if(isalpha(linea[i]) || linea[i] == 39){
	
				/* 
					Concatenate a char to form a word if
 					it is a letter or a aphostrophe mark
				 */
				subcadena[j] = tolower(linea[i]);
				j++;
	
			}else if (isspace(linea[i]) || linea[i] == 45){
	
				/*
					We arrive at the end of a word if 
					it is a space or a minus mark
					Incr the number of words.
					Insert it at the table hash
				 */
				if (j > 0 && !isOdd){
					npalabras ++;
					insertHashTable(subcadena, tablaHash);
				}

				/* Reinitialize the var subcadena */
				memset(subcadena, 0, MAXWORD);
				j = 0;
				isOdd = 0;

			}else if(ispunct(linea[i])){

				if(i == (len-1)){
					if (linea[i-1] != ' ' && !isOdd){

						/*
							If we arrive here, that means that the we have 
							the last word of the line, and we have to
							insert it at the hash table and in the RBD tree.
						 */
						npalabras ++;
						insertHashTable(subcadena, tablaHash);
					}
				}

			}else{

				/* 
					If the char is a odd word, 
					we activate the flag to be aware of it
				 */
				isOdd = 1;

			}
		}

		//printf("\n");

		//printf("Has extret %d paraules de la linea.\n", npalabras);
	}
}