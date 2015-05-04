/**
 *
 * Practica 4 of Operative Systems 2.
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
#include <sys/stat.h>
#include <sys/time.h>
#include <pthread.h>
#include "linked-list.h"
#include "red-black-tree.h"

int menuPrincipal();
int menuAnalisisArbre();
void creacioArbre(RBTree * tree);
void enmagatzamentArbre(RBTree * tree);
void lecturaArbre(RBTree * tree);
void probabilitat_Segons_Fitxer(RBTree * tree);
void probabilitat_Segons_Nlletres(RBTree * tree);

void insert_AtRBTree_multiThread(RBTree * tree, FILE * configFile);
int funcHash(char *cadena);
void insertNewWord_AtRBTree(RBTree * tree, char *subcadena, int * vector);
void insertWord_AtRBTree(RBTree * tree, char *subcadena, int numTimes, int numFile);
void insertWord_HashTable(List * tablaHash, char * subcadena);
void fromHash_toRBTree(RBTree * tree, List * tablaHash, int numFile);
void doLine(List * tablaHash, char *linea, char * subcadena);
List * fromFile_toHash(char *filename);

#define MAXLINEA 200
#define SIZE 100

#define MAXTHREADS 4	// Modify this to choice how many threads use

int n_nodes = -1;
int n_files = -1;
int max_word = -1;

int numFile;
int acaba;
int contador;

pthread_t ntid[MAXTHREADS];
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex3 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex4 = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t  cond  = PTHREAD_COND_INITIALIZER;
pthread_cond_t  cond_2  = PTHREAD_COND_INITIALIZER;

struct parametres {
	RBTree * tree;
	FILE * configFile;
};

struct parametres * par;

/**
 *
 * Function used to prints the main menu.
 *
 */

int menuPrincipal(){

	int opcio;

	printf("\nPractica 4\n");
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
	
	/* Chronological Time */
	struct timeval tv1, tv2;
	/* CPU Time */
	clock_t t1, t2;

	printf("Fitxer de configuracio de la llista de fitxers a analitzar?\n");
	printf(">>> ");
	scanf("%s", path);

  	configFile = fopen(path, "r");
	
	if (configFile == NULL){
		printf("No s'ha pogut carregar el fitxer de configuracio.\n");
		
	}else{
		printf("S'ha carregat el fitxer de configuracio.\n");
		printf("Creant l'arbre...\n");

		gettimeofday(&tv1, NULL);
		t1 = clock();
		
		insert_AtRBTree_multiThread(tree, configFile);
		
		gettimeofday(&tv2, NULL);
		t2 = clock();

		printf("\nS'han insert %d nodes\n", n_nodes);
		printf("De %d fitxers.\n", n_files);
		printf("La paraula més llarga té %d lletres.\n", max_word);
		
		printf("Temps de CPU: %f seconds\n", (double)(t2 - t1) / (double) CLOCKS_PER_SEC);
		printf("Temps cronologic = %f seconds\n", (double) (tv2.tv_usec - tv1.tv_usec) / 1000000 + (double) (tv2.tv_sec - tv1.tv_sec));
		
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

	int nelems, node;
	char * key;
	int * vector;

	fp = fopen(path, "r");

	if (!fp){
		printf("ERROR, no s'ha pogut obrir el fitxer\n");
		exit(EXIT_FAILURE);
	}

	/* If exist a tree, delete it */
	if (n_nodes != -1){
		deleteTree(tree);
		printf("S'ha esborrat l'arbre anterior, s'inicialitzarà un de nou.\n");
	}
	
	/* Initialize the Tree */
	initTree(tree);
	printf("\tNou arbre inicialitzat!!\n");

  	n_nodes = 0;

	/* Get the number of nodes, files and maximum word size from the file */
	
	nelems = fread(&n_nodes, sizeof(int), 1, fp);
	if (nelems != 1){
		printf("ERROR, el fitxer és massa curt.\n");
		exit(EXIT_FAILURE);
	}

	if (n_nodes <= 0){
		printf("ERROR, la mida n_nodes no es correcte.\n");
		exit(EXIT_FAILURE);
	}

	nelems = fread(&n_files, sizeof(int), 1, fp);
	if (nelems != 1){
		printf("ERROR, el fitxer és massa curt.\n");
		exit(EXIT_FAILURE);
	}

	if (n_files <= 0){
		printf("ERROR, la mida n_files no es correcte.\n");
		exit(EXIT_FAILURE);
	}

	nelems = fread(&max_word, sizeof(int), 1, fp);
	if (nelems != 1){
		printf("ERROR, el fitxer és massa curt.\n");
		exit(EXIT_FAILURE);
	}

	if (max_word <= 0){
		printf("ERROR, la mida max_word no es correcte.\n");
		exit(EXIT_FAILURE);
	}

	printf("Hi han %d nodes guardats.\n", n_nodes);
	printf("S'han llegit %d fitxers.\n", n_files);
	printf("La paraula més llarga te %d lletres.\n", max_word);

	key = malloc(sizeof(char)*MAXWORD);
	vector = malloc(sizeof(int)*n_files);

	/* Bucle to go over all nodes of the file */
	node = 0;
	while (node < n_nodes){
		
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
		
		insertNewWord_AtRBTree(tree, key, vector);

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

	char * paraula = malloc(sizeof(char) * MAXLINEA);
	
	printf("Paraula a buscar a l'arbre?\n");
	printf(">>> ");
	scanf("%s", paraula);

	/* Search if the word is in the tree */
	Node *node = findNode(tree, paraula);

	if (node != NULL){

		RBdata *data = node->data;

		float * prob = malloc(sizeof(float) * n_files);

		printf("La paraula %s surt un total de %d vegades.\n", data->key, data->num);

		/* Calculate and print the probability in each file with a bucle */
		for (i = 0; i < n_files; i++){
				printf("Al fitxer %d surt %d vegades,", i, data->vector[i]);
				prob[i] = ((float) data->vector[i]) / ((float) data->num);
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
 * Function that execute every thread created for create a rbtree
 *
 */

void * thr_fn_2(void *arg){

	/* Sleep the created threads and wake up the main thread to print menu */
	pthread_mutex_lock(&mutex);

		contador ++;
		/*
		printf("Adormint fil nº %d\n", contador);
		fflush(stdout);
		*/
		if (contador == MAXTHREADS)
			pthread_cond_signal(&cond_2);
		pthread_cond_wait(&cond, &mutex);
		
	pthread_mutex_unlock(&mutex);


	while (!acaba){

		while (numFile < n_files){

			char * linea_path = malloc(sizeof(char) * MAXLINEA);
			if (linea_path == NULL){
		    	printf("No he pogut reservar memòria\n");
				exit(1);
		  	}

			/* Assign a file to a thread */
			pthread_mutex_lock(&mutex);
				fgets(linea_path, MAXLINEA, par->configFile);
				int n = numFile;
				printf("Procesant fitxer %d\n", n); fflush(stdout);
				numFile ++;
			pthread_mutex_unlock(&mutex);

			if (n >= n_files){
				free(linea_path);
				break;
			}

			/* Insert words of a file in a hash table */
			List * tablaHash = fromFile_toHash(linea_path);

			/* Insert data of a hash table in a tree */
			pthread_mutex_lock(&mutex2);
				fromHash_toRBTree(par->tree, tablaHash, n);
			pthread_mutex_unlock(&mutex2);

			int i;
			for(i = 0; i < SIZE; i++){
				deleteList(&tablaHash[i]);
			}
			free(tablaHash);
			free(linea_path);
		}

		/* When threads dont have more files to process, we sleep them
		   When all threads are sleeping, wake up the main thread	*/
		pthread_mutex_lock(&mutex4);
			contador --;
			if (contador == 0){
				pthread_cond_signal(&cond_2);
			}
			pthread_cond_wait(&cond, &mutex4);
		pthread_mutex_unlock(&mutex4);

	}
	return((void *)0);
}

/**
 *
 * Function that inserts the configuration file, where every line 
 * is a path to read another file where there it is the words to be read, 
 * to the tree. Using various threads.
 *
 */

void insert_AtRBTree_multiThread(RBTree * tree, FILE * configFile){

	/* Initialize the Tree */
  	initTree(tree);

  	n_nodes = 0;
	max_word = 0;

	char * linea_path = malloc(sizeof(char) * MAXLINEA);
	if (linea_path == NULL){
    	printf("No he pogut reservar memòria\n");
		exit(1);
  	}

	/* Get the first line of the file of paths.*/
	fgets(linea_path, MAXLINEA, configFile);
	n_files = atoi(linea_path);
	free(linea_path);

	/* Save tree and configFile in a structure */
	par->tree = tree;
	par->configFile = configFile;

	numFile = 0;

	/* Wake up the threads to process files and sleep the main thread */
	pthread_mutex_lock(&mutex);
		pthread_cond_broadcast(&cond);
		pthread_cond_wait(&cond_2, &mutex);
	pthread_mutex_unlock(&mutex);

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
 * This func insert a string at the RB Tree
 * (used by creacioArbre())
 *
 */

void insertWord_AtRBTree(RBTree * tree, char *subcadena, int numTimes, int nFile){
	
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
		data->vector[nFile] += numTimes;

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

		data->vector[nFile] = numTimes;

		insertNode(tree, data);

		n_nodes ++;
	}
}


/**
 *
 * This func insert a string at the RB Tree
 * (used by lecturaArbre())
 *
 */

void insertNewWord_AtRBTree(RBTree * tree, char *subcadena, int * vector){

	RBdata *data;

	int i;

	data = malloc(sizeof(RBdata));

	data->key = malloc(sizeof(char)*MAXWORD);

	strcpy(data->key, subcadena);

	int numTimes = 0;
	data->vector = malloc(sizeof(int)*n_files);

	for (i = 0; i < n_files; i++){
		data->vector[i] = vector[i];
		if (vector[i] > 0){
			numTimes ++;
		}
	}
	data->num = numTimes;

	insertNode(tree, data);
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

	if ( n_lletres > max_word ){
		pthread_mutex_lock(&mutex3);
		max_word = n_lletres;
		pthread_mutex_unlock(&mutex3);
	}

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

void fromHash_toRBTree(RBTree * tree, List * tablaHash, int nFile){
	int i;
	struct ListItem_ * centinela;

	char * cadena = malloc(sizeof(char)*MAXWORD);
	memset(cadena, 0, MAXWORD);
	
	/* Bucle to go over the hash table */
	for(i = 0; i < SIZE; i++){
	  
		centinela = tablaHash[i].first;
	
		while(centinela){
			insertWord_AtRBTree(tree, centinela->data->key, centinela->data->numTimes, nFile);
 			centinela = centinela->next;
		}
	}

	free(cadena); 
}

/**
 *
 * This func treats a file to extract the in words and
 * furthermore insert them at the table hash.
 *
 */

List * fromFile_toHash(char *filename){

	FILE *fp;

	struct stat st;

	char *bytes, *paraula_copy;
	int i, j, nbytes, is_word, done;

	char * path = malloc(sizeof(char) * MAXLINEA);
	if (path == NULL){
    	printf("No he pogut reservar la memòria\n");
		exit(1);
	}

	char * paraula = malloc(sizeof(char) * MAXWORD);
	if (paraula == NULL){
		printf("No he pogut reservar la memòria\n");
		exit(1);
	}

	/* Initialize the table hash */
	List * tablaHash;

	tablaHash = malloc(sizeof(List) * SIZE);
	if (tablaHash == NULL){
		printf("No he pogut reservar la memòria\n");
		exit(1);
	}

	/* Every hash table position we initialize it with the linked list */
	for(i = 0; i < SIZE; i++){
		initList(&tablaHash[i]);
	}

	memset(path, 0, MAXLINEA);
	i = 0;
	
	while(filename[i] != 10 && i < strlen(filename)){
		/* 
		   Here we copy the path file line without
		   the last character LF (new line) code ASCII
		*/
		path[i] = filename[i];
		i ++;
	}

	fp = fopen(path, "r");
	if (!fp) {
		printf("No s'ha pogut obrir el fitxer '%s'\n", path);
	}

	stat(path, &st);
	nbytes = st.st_size;

	free(path);

	bytes = malloc(sizeof(char) * nbytes);
	if (!bytes) {
		printf("No he pogut reservar memoria.\n");
		exit(1);
	}

	fread(bytes, sizeof(char), nbytes, fp);
	fclose(fp);


	/*LICENSED BY LLUIS GARRIDO*/
	i = 0;
	while (i < nbytes){
		while ((i < nbytes) && (isspace(bytes[i]) || (ispunct(bytes[i]) && (bytes[i] != '\''))) ) i++; 

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

				insertWord_HashTable(tablaHash, paraula_copy);

				free(paraula_copy);

			} /* if is_word */

		} /* if isalpha(bytes[i]) */ 
	} /* while */
	
	free(paraula);
	free(bytes);

	return tablaHash;
}

int main(){

	RBTree tree;
	int opcio, i, err;
	par = malloc(sizeof(struct parametres));

	acaba = 0;
	contador = 0;

	pthread_mutex_lock(&mutex);

	/* Create threads */
	for (i = 0; i < MAXTHREADS; i++){
		err = pthread_create(ntid+i, NULL, thr_fn_2, NULL);
		if (err!=0){
			printf("No puc crear el fil numero %d\n", i);
			exit(1);
		}
	}

	/* Sleep the main thread while we sleep the threads created */
	pthread_cond_wait(&cond_2, &mutex);
	
	pthread_mutex_unlock(&mutex);

	/* Main thread will wait here until all threads are sleeping */
	pthread_mutex_lock(&mutex);
	pthread_mutex_unlock(&mutex);

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

	/* Wake up the threads to finish  */
	acaba = 1;
	pthread_cond_broadcast(&cond);

	/* Wait active threads */
	for(i = 0; i < MAXTHREADS; i++) {
		err = pthread_join(ntid[i], NULL);
		if (err != 0) {
			printf("error pthread_join al fil %d\n", i);
			exit(1);
		}
	}

	/*Release the tree memory*/
	if(n_nodes != -1){
		printf("\nSortint del programa... \n\t\bEliminant arbre!!\n");
		deleteTree(&tree);
	}

	printf("\nFins aviat!\n");

	free(par);
	
	return 0;
}