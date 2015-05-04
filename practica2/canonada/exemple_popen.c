#include <stdio.h>
#include <stdlib.h>

#define MAXLINE 100

int main(int argc, char *argv[])
{
  char	line[MAXLINE];
  FILE	*fpin, *fpout;

  if (argc != 2)
  {
    printf("us: %s fitxer\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  fpin = fopen(argv[1], "r");
  if (!fpin) {
    printf("ERROR: no puc obrir fitxer d'entrada.\n");
    exit(EXIT_FAILURE);
  }

  fpout = popen("less", "w");
  if (!fpout)
  {
    printf("ERROR: no puc crear canonada.\n");
    exit(EXIT_FAILURE);
  }

  /*  copiem el contingut de argv[1]
      a la canonada */
  while (fgets(line, MAXLINE, fpin) != NULL) {
    if (fputs(line, fpout) == EOF) {
      printf("ERROR: no puc escriure a la canonada.\n");
      exit(EXIT_FAILURE);
    }
  }

  if (pclose(fpout) == -1)
  {
    printf("ERROR: pclose.\n");
    exit(EXIT_FAILURE);
  }

  printf("L'aplicació ha acabat.\n");

  return 0;
}