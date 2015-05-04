#include <stdlib.h>
#include <stdio.h>


int main(int argc, char *argv[]){

	FILE *fp;

	fp = popen("gnuplot","w");

	fprintf(fp, "plot \"grafica2d.data\" with lines\n");

	fflush(fp);

	printf("\nEnviat....\n");

	sleep(5);

	pclose(fp);

	return 0;
}
