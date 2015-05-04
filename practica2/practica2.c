/**
 *
 * Practica 2 of Operative Systems 2.
 *
 * Dennis Quitaquis
 * Daniel Rivero
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <ctype.h>
#include <unistd.h>
#include "linked-list.h"
#include "red-black-tree.h"

int menuPrincipal();
int menuAnalisisArbre();
void creacioArbre(RBTree * tree);
void enmagatzamentArbre(RBTree * tree);
void lecturaArbre(RBTree * tree);
void probabilitat_Segons_Fitxer(RBTree * tree);
void probabilitat_Segons_Nlletres(RBTree * tree);

void insert_AtRBTree(RBTree * tree, FILE * configFile);
int funcHash(char *cadena);
void insertWord_AtRBTree(RBTree * tree, char *subcadena, int numTimes, int numFile);
void insertWord_HashTable(List * tablaHash, char * subcadena);
void fromHash_toRBTree(RBTree * tree, List * tablaHash, int numFile);
void doLine(List * tablaHash, char *linea, char * subcadena);

#define MAXLINEA 200
#define SIZE 100

int n_nodes = -1;
int n_files = -1;
int max_word = -1;

int main(){

	RBTree tree;

	int opcio;

	/* Prints the menu */
	
	do {

		opcio = menuPrincipal();

		switch(opcio){
			case 1:
				printf("\nOpcio1\n");
				creacioArbre(&tree);
				break;
			case 2:
				printf("\nOpcio2\n");
				enmagatzamentArbre(&tree);
				break;
			case 3:
				printf("\nOpcio3\n");
				lecturaArbre(&tree);
				break;
			case 4:
				printf("\nOpcio4\n");

				opcio = menuAnalisisArbre();

				switch(opcio){
					case 1:
						printf("\nSub-opcio1\n");
						probabilitat_Segons_Fitxer(&tree);
						break;
					case 2:
						printf("\nSub-opcio2\n");
						probabilitat_Segons_Nlletres(&tree);
						break;
				}

				break;
		}

	}while(opcio != 5);

	printf("\nFins aviat!\n");

	/*Release the tree memory*/
	if(n_nodes != -1)
		deleteTree(&tree);
	
	return 0;
}

/**
 *
 * Function used to prints the main menu.
 *
 */

int menuPrincipal(){

	int opcio;

	printf("\nPractica 2\n");
	printf("\t1- Creacio de l'arbre.\n");
	printf("\t2- Enmagatzament de l'arbre.\n");
	printf("\t3- Lectura de l'arbre.\n");
	printf("\t4- Analisis de paraules de l'arbre.\n");
	printf("\t5- Sortida.");

	printf("\n>>> ");
	scanf("%d", &opcio);

	while (opcio < 1 || opcio > 5){
		printf("\n>>> ");
		scanf("%d", &opcio);
	}

	return opcio;
}

/**
 *
 * Function used to prints the sub-menu of option 4.
 *
 */

int menuAnalisisArbre(){

	int opcio;

	printf("\nAnàlisis de l'arbre\n");
	printf("\t1- Dibuixar la probabilitat d'una determinada\n\t   paraula segons el fitxer.\n");
	printf("\t2- Dibuixar la probabilitat d'aparició de les\n\t   paraules als textos segons el nombre de lletres que tenen.\n");
	printf("\t3- Enrere.");

	printf("\n>>> ");
	scanf("%d", &opcio);

	while (opcio < 1 || opcio > 3){
		printf("\n>>> ");
		scanf("%d", &opcio);
	}

	return opcio;
}

/**
 *
 * This func create a RBTree with the data of a config file.
 *
 */
 
void creacioArbre(RBTree * tree){

	FILE * configFile;

	char * path = malloc(sizeof(char) * MAXLINEA);

	printf("Fitxer de configuracio de la llista de fitxers a analitzar?\n");
	printf(">>> ");
	scanf("%s", path);

  	configFile = fopen(path, "r");
	
	if (configFile == NULL){
		printf("No s'ha pogut carregar el fitxer de configuracio.\n");
		
	}else{
		printf("S'ha carregat el fitxer de configuracio.\n");
		printf("Creant l'arbre...\n");
		insert_AtRBTree(tree, configFile);
		printf("S'han insert %d nodes\n", n_nodes);
		printf("De %d fitxers.\n", n_files);
		printf("La paraula més llarga té %d lletres.\n", max_word);
		fclose(configFile);
	}
	
	/* Release memory of the file name */
	free(path);
	
}

/**
 *
 * This function store a current tree in a file.
 *
 */

void enmagatzamentArbre(RBTree * tree){

	char * path = malloc(sizeof(char) * MAXLINEA);
	
	printf("Nom del fitxer on vol desar l'arbre?\n");
	printf(">>> ");
	scanf("%s", path);

	if (n_nodes != -1){
		storeRBTree(tree, path, n_nodes, n_files, max_word);
		printf("Arbre enmagatzemat correctament!\n");
	}else{
		printf("Crea o carrega primer un arbre!\n");
	}

	/* Release memory of the file name */
	free(path);
}

/**
 *
 * This function load a tree from a file.
 *
 */

void lecturaArbre(RBTree * tree){

	char * path = malloc(sizeof(char) * MAXLINEA);
	
	printf("Nom del fitxer amb l'arbre a llegir?\n");
	printf(">>> ");
	scanf("%s", path);

	FILE *fp;

	int i, nelems, nnodes, node;
	char * key;
	int * vector;

	fp = fopen(path, "r");

	if (!fp){
		printf("ERROR, no s'ha pogut obrir el fitxer\n");
		exit(EXIT_FAILURE);
	}

	/* If exist a tree, delete */
	if (n_nodes != -1){
		deleteTree(tree);
		printf("S'ha esborrat l'arbre anterior, s'inicialitzarà un de nou.\n");
	}
	
	/* Initialize the Tree */
	initTree(tree);

  	n_nodes = 0;

	/* Get the number of nodes, files and maximum word size from the file */
	
	nelems = fread(&nnodes, sizeof(int), 1, fp);
	if (nelems != 1){
		printf("ERROR, el fitxer és massa curt.\n");
		exit(EXIT_FAILURE);
	}

	if (nnodes <= 0){
		printf("ERROR, la mida no es correcte.\n");
		exit(EXIT_FAILURE);
	}

	nelems = fread(&n_files, sizeof(int), 1, fp);
	if (nelems != 1){
		printf("ERROR, el fitxer és massa curt.\n");
		exit(EXIT_FAILURE);
	}

	if (n_files <= 0){
		printf("ERROR, la mida no es correcte.\n");
		exit(EXIT_FAILURE);
	}

	nelems = fread(&max_word, sizeof(int), 1, fp);
	if (nelems != 1){
		printf("ERROR, el fitxer és massa curt.\n");
		exit(EXIT_FAILURE);
	}

	if (max_word <= 0){
		printf("ERROR, la mida no es correcte.\n");
		exit(EXIT_FAILURE);
	}

	printf("Hi han %d nodes guardats.\n", nnodes);
	printf("S'han llegit %d fitxers.\n", n_files);
	printf("La paraula més llarga te %d lletres.\n", max_word);

	key = malloc(sizeof(char)*MAXWORD);
	vector = malloc(sizeof(int)*n_files);

	/* Bucle to go over all nodes of the file */
	node = 0;
	while (node < nnodes){
		
		nelems = fread(key, sizeof(char), MAXWORD, fp);
		if (nelems != MAXWORD ){
			printf("ERROR, llegint el fitxer, no corresponen les mides key.\n");
			exit(EXIT_FAILURE);
		}

		nelems = fread(vector, sizeof(int), n_files, fp);
		if (nelems != n_files ){
			printf("ERROR, llegint el fitxer, no corresponen les mides vector.\n");
			exit(EXIT_FAILURE);
		}

		// printf("La paraula: %s ", key);
		// printf("te com a vector: ");
		
		for (i = 0; i < n_files; i++){

			// printf("%d ", vector[i]);
			
			insertWord_AtRBTree(tree, key, vector[i], i);

		}
		// printf("\n");

		/* Reinitialize the var key and vector */
		memset(key, 0, MAXWORD);
		memset(vector, 0, n_files);

		node ++;
	}

	/* Close file and release memory */
	fclose(fp);
	free(key);
	free(vector);
	free(path);
}

/**
 * 
 * This function is called by the qsort function. It is used to compare two
 * elements of the vector to order.
 *
 */

static int compare_floats(const void *p1, const void *p2){
	float *num1, *num2;

	num1 = (float *) p1;
	num2 = (float *) p2;

	if (*num1 < *num2)
		return 1;
	if (*num1 > *num2)
		return -1;
	return 0;
}

/**
 *
 * This function calculate the probability to appear a word in each file
 *
 */

void probabilitat_Segons_Fitxer(RBTree * tree){

	int i;
	Node *node;
	RBdata *data;

	char * paraula = malloc(sizeof(char) * MAXLINEA);
	
	printf("Paraula a buscar a l'arbre?\n");
	printf(">>> ");
	scanf("%s", paraula);

	/* Search if the word is in the tree */
	node = findNode(tree, paraula);

	if (node != NULL){

		float * prob = malloc(sizeof(float) * n_files);

		data = node->data;

		printf("La paraula %s surt un total de %d vegades.\n", data->key, data->num);

		/* Calculate and print the probability in each file with a bucle */
		for (i = 0; i < n_files; i++){
			printf("Al fitxer %d surt %d vegades,", i, data->vector[i]);
			prob[i] = (float) data->vector[i] / (float) data->num;
			printf(" prob: %f \n", prob[i]);
		}

		/* Sort the probabilities */
		qsort(prob, n_files, sizeof(float), compare_floats);

		FILE *fp_data, *fp_gnuplot;

		fp_data = fopen("vector.data","w");

		if (!fp_data){
			printf("ERROR: no he pogut obrir el fitxer.\n");
			exit(EXIT_FAILURE);
		}

		printf("The ordered vector is: ");

		/* Store probabilities in a file */
		
		fprintf(fp_data, "%s\n", "#Funcio prob(paraula)" );
		fprintf(fp_data, "%s\n", "#x   prob(x)" );

		for(i = 0; i < n_files; i++){
			fprintf(fp_data, "%d %f\n", i, prob[i]);
			printf("%f ", prob[i]);
		}

		printf("\n");
		
		/* Close file and release memory */
		fclose(fp_data);
		free(prob);

		/* Print a graphic of probs using gnuplot with a pipe */
		
		fp_gnuplot = popen("gnuplot", "w");
		if (!fp_gnuplot){
			printf("ERROR: no he pogut obrir el fitxer.\n");
			exit(EXIT_FAILURE);
		}

		fprintf(fp_gnuplot, "set xlabel \"apareix en el fitxer x\"\n");
		fprintf(fp_gnuplot, "set ylabel \"amb probabilitat y\"\n");

		fprintf(fp_gnuplot, "plot \"vector.data\" with lines title \"la paraula %s\"\n", data->key);
		fflush(fp_gnuplot);

		printf("\nEnviat....\n");

		sleep(10);

		pclose(fp_gnuplot);	

	}else{

		printf("No s'ha trobat la paraula a l'arbre!");

	}

	/* Release memory */
	free(paraula);
}

/**
 *
 * This function calculate the probability to appear a word in the files depending of her length
 *
 */

void probabilitat_Segons_Nlletres(RBTree * tree){

	int i;

	int * prob = malloc(sizeof(int)*max_word);
	for (i = 0; i < max_word; i++){
		prob[i] = 0;
	}

	/* Call a function that count how many words with n letters appear in the files 
		and return a vector with probability of each word length */
	probabilitatLletres(tree, prob);

	for (i = 0; i < max_word; i++){
		printf("Paraula amb %d lletres: %d\n", i+1, prob[i]);
	}

	FILE *fp_data, *fp_gnuplot;

	fp_data = fopen("vector.data","w");

	if (!fp_data){
		printf("ERROR: no he pogut obrir el fitxer.\n");
		exit(EXIT_FAILURE);
	}

	/* Store probabilities in a file */
	
	fprintf(fp_data, "%s\n", "#Funcio" );
	fprintf(fp_data, "%s\n", "#n lletres   paraules amb n lletres" );

	for(i = 0; i < max_word; i++){
		fprintf(fp_data, "%d %d\n", i+1, prob[i]);
	}

	/* Close file and release memory */
	fclose(fp_data);
	free(prob);

	/* Print a graphic of probs using gnuplot with a pipe */
	
	fp_gnuplot = popen("gnuplot", "w");
	if (!fp_gnuplot){
		printf("ERROR: no he pogut obrir el fitxer.\n");
		exit(EXIT_FAILURE);
	}

	fprintf(fp_gnuplot, "set xlabel \"x lletres\"\n");
	fprintf(fp_gnuplot, "set ylabel \"y paraules\"\n");
	fprintf(fp_gnuplot, "plot \"vector.data\" with lines title \"Prob(paraula) al text segons el # lletres\"\n");
	fflush(fp_gnuplot);

	printf("\nEnviat....\n");

	sleep(10);

	pclose(fp_gnuplot);	
}

/*************************************************
 *
 * OTHER FUNCTIONS
 *
 ************************************************/

/**
 *
 * Function that inserts the configuration file, where every line 
 * is a path to read another file where there it is the words to be read, 
 * to the tree.
 *
 */

void insert_AtRBTree(RBTree * tree, FILE * configFile){

	/* Initialize the Tree */
  	initTree(tree);

  	n_nodes = 0;
	n_files = 0;
	max_word = 0;

	FILE *textFile;

	int i, k, numFile;

	char * linea_path = malloc(sizeof(char) * MAXLINEA);
	if (linea_path == NULL){
    	printf("No he pogut reservar la memòria\n");
		exit(1);
  	}

  	char * path = malloc(sizeof(char) * MAXLINEA);
	if (path == NULL){
	    	printf("No he pogut reservar la memòria\n");
			exit(1);
	}

	char * linea_file = malloc(sizeof(char) * MAXLINEA);
	if (linea_file == NULL){
    	printf("No he pogut reservar la memòria\n");
		exit(1);
  	}

  	char * subcadena = malloc(sizeof(char) * MAXWORD);
	if (subcadena == NULL){
    	printf("No he pogut reservar la memòria\n");
		exit(1);
  	}

	/* Get the first line of the file of paths.*/
	fgets(linea_path, MAXLINEA, configFile);
	n_files = atoi(linea_path);

	numFile = 0;

	/*
		In this bucle we are going to read all the path files.
	*/
	while(fgets(linea_path, MAXLINEA, configFile) != NULL && numFile < n_files){
		/* Reinitialize the var path */
		memset(path, 0, MAXLINEA);

		k = 0;
		while(linea_path[k] != 10){
			/* Here we copy the path file line without the last character LF (new line) code ASCII */
			path[k] = linea_path[k];
			k ++;
		}

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
			doLine(tablaHash, linea_file, subcadena);
		}

		/* We close the text file*/
		fclose(textFile);
		fromHash_toRBTree(tree, tablaHash, numFile);

		/*Release memory for the hash table. */
		for(i = 0; i < SIZE; i++){
			deleteList(&tablaHash[i]);
		}
		free(tablaHash);

		numFile ++;

	}

	free(linea_path);
	free(path);
	free(linea_file);
	free(subcadena);
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

void insertWord_AtRBTree(RBTree * tree, char *subcadena, int numTimes, int numFile){
	
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
		data->vector[numFile] += numTimes;

	}else{

		/* 
			If the key is not in the tree, allocate memory for the data
       		and insert in the tree
       	 */

		data = malloc(sizeof(RBdata));

		data->key = malloc(sizeof(char)*MAXWORD);
		memset(data->key, 0, MAXWORD);

		strcpy(data->key, subcadena);

		data->num = numTimes;
		data->vector = malloc(sizeof(int)*n_files);

		for (i = 0; i < n_files; i++){
			data->vector[i] = 0;
		}

		data->vector[numFile] = numTimes;

		insertNode(tree, data);

		n_nodes ++;
	}
}

/**
 *
 * This func insert a string at the hash table
 *
 */

void insertWord_HashTable(List * tablaHash, char * subcadena){

	int valor_hash, n_lletres;

	ListData *listData;

	n_lletres = (int)strlen(subcadena);

	if ( n_lletres > max_word )
		max_word = n_lletres;

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
}

/**
 *
 * This func gets the hash table and for every word in it,
 * it inserts at the RBD Tree.
 *
 */

void fromHash_toRBTree(RBTree * tree, List * tablaHash, int numFile){
	int i;
	struct ListItem_ * centinela;

	char * cadena = malloc(sizeof(char)*MAXWORD);
	memset(cadena, 0, MAXWORD);
	
	/* Bucle to go over the hash table */
	for(i = 0; i < SIZE; i++){
	  

		centinela = tablaHash[i].first;
	
		while(centinela){
			strcpy(cadena, centinela->data->key);

			insertWord_AtRBTree(tree, cadena, centinela->data->numTimes, numFile);  
			
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

void doLine(List * tablaHash, char *linea, char * subcadena){

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
					insertWord_HashTable(tablaHash, subcadena);
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
						insertWord_HashTable(tablaHash, subcadena);
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

		//printf("Has extret %d paraules de la linea.\n", npalabras);
	}
}

