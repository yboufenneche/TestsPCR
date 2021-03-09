#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/wait.h>
#include "lectureEcriture.h"
#include "alea.h"
#include "message.h"
#include "memoire.h"

#define CCENTRE "0001"

int main(int argc, char **argv)
{
  int fdtToa[2], fdaTot[2], fdaTov, fdvToa, fdiToa, fdaToi;
  int fd;

  fdtToa[0] = open("t1Toa.txt", O_RDONLY);
  fdtToa[1] = open("t2Toa.txt", O_RDONLY);
  fdaTot[0] = open("aTot1.txt", O_WRONLY);
  fdaTot[1] = open("aTot2.txt", O_WRONLY);

  fdvToa = open("vToa.txt", O_RDONLY);
  fdaTov = open("aTov.txt", O_WRONLY);
  fdiToa = open("iToa.txt", O_RDONLY);
  fdaToi = open("aToi.txt", O_WRONLY);

  char *ligne, *mes;
  char nTest[17], type[7], valeur[10];

  //int decoupeOk;
  int nbTerm, tailleMem;
  char code[5];

  nbTerm = atoi(argv[1]);
  //tailleMem = atoi(argv[2]);

  tra_t *memoire = calloc(tailleMem, sizeof(tra_t));
  tra_t e;

  /*
      traiter les terminaux
    */
  int i = 0;
  while (i < nbTerm)
  {
    ligne = litLigne(fdtToa[i]);
    mes = suppRetourChariot(ligne);
    decoupe(mes, nTest, type, valeur);
    strncpy(code, nTest, 4);

    sprintf(e.nTest, "%s", nTest);
    sprintf(e.ter, "%d", fdaTot[i]);
    ajouterEntree(memoire, e);

    if (strcmp(code, CCENTRE) == 0)
    {
      ecritLigne(fdaTov, ligne);
      ecritLigne(fdaTov, "\n");
    }
    else
    {
      ecritLigne(fdaToi, ligne);
      ecritLigne(fdaToi, "\n");
    }
    i++;
  }

  /*
      traiter les serveurs de validation
    */
  while ((ligne = litLigne(fdvToa)) != NULL)
  {
    decoupe(ligne, nTest, type, valeur);
    printf("%s", ligne);
    fd = atoi(trouverEntree(memoire, nTest));
    ecritLigne(fd, ligne);
  }

  /*
      traiter les serveurs InterArchive
    */
  while ((ligne = litLigne(fdiToa)) != NULL)
  {
    decoupe(ligne, nTest, type, valeur);
    printf("%s", ligne);
    fd = atoi(trouverEntree(memoire, nTest));
    ecritLigne(fd, ligne);
  }

  return 0;
}