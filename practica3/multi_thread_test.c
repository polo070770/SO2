void * thr_fn(void *arg){

	int i;

	struct parametres *par = (struct parametres * ) arg;

	while (numFile < n_files){

		pthread_mutex_lock(&mutex);
			char * linea_path = malloc(sizeof(char) * MAXLINEA);
			if (linea_path == NULL){
		    	printf("No he pogut reservar memòria\n");
				exit(1);
		  	}

			fgets(linea_path, MAXLINEA, par->configFile);		

			numFile ++;
		pthread_mutex_unlock(&mutex);

		List * tablaHash = fromFile_toHash(linea_path);

		pthread_mutex_lock(&mutex2);
			int n = numFile;
			fromHash_toRBTree(par->tree, tablaHash, n);
		pthread_mutex_unlock(&mutex2);

		for(i = 0; i < SIZE; i++){
			deleteList(&tablaHash[i]);
		}
		free(tablaHash);
		free(linea_path);
	}

	return((void *)0);
}