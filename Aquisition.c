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

/*
* recevoir les demandes des treminaux
*/
void *traiterTerminal(){

}

/*
* recevoir les réponses du serveur de validation
*/
void *traiterValidation(){

}

/*
* recevoir les demandes et les répondes du serveur InterArchive
*/
void *traiterInterArchive(){

}

int main(int argc, char **argv)
{
  int fdtToa[3], fdaTot[3], fdaTov, fdvToa, fdiToa, fdaToi;
  int fd;

  fdtToa[0] = open("t1Toa.txt", O_RDONLY);
  fdtToa[1] = open("t2Toa.txt", O_RDONLY);
  fdtToa[2] = open("t3Toa.txt", O_RDONLY);

  fdaTot[0] = open("aTot1.txt", O_WRONLY);
  fdaTot[1] = open("aTot2.txt", O_WRONLY);
  fdaTot[2] = open("aTot3.txt", O_WRONLY);

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
  tailleMem = atoi(argv[2]);

  tra_t *memoire = calloc(tailleMem, sizeof(tra_t));
  tra_t e;

  /*
      traiter les terminaux
    */
  int i = 0;
  while (i < nbTerm)
  {
    printf("\nTraitement de Terminal %d...\n", i+1);
    while ((ligne = litLigne(fdtToa[i])) != NULL)
    {
      printf("%s", ligne);
      mes = suppRetourChariot(ligne);
      decoupe(mes, nTest, type, valeur);
      strncpy(code, nTest, 4);

      sprintf(e.nTest, "%s", nTest);
      sprintf(e.fdesc, "%d", fdaTot[i]);
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
    }
    i++;
  }

  /*
      traiter les serveurs de validation
    */
  printf("\nTraitement du serveur Validation...\n");
  while ((ligne = litLigne(fdvToa)) != NULL)
  {
    printf("%s", ligne);
    decoupe(ligne, nTest, type, valeur);
    fd = atoi(trouverEntree(memoire, nTest));
    ecritLigne(fd, ligne);
  }

  /*
      traiter les serveurs InterArchive
    */
  printf("\nTraitement du serveur InterArchive...\n");
  while ((ligne = litLigne(fdiToa)) != NULL)
  {
    printf("%s", ligne);
    decoupe(ligne, nTest, type, valeur);
    if (strcmp(valeur, "Reponse") == 0)
    {
      fd = atoi(trouverEntree(memoire, nTest));
      ecritLigne(fd, ligne);
    }
    // else{
    //   sprintf(e.nTest, "%s", nTest);
    //   sprintf(e.fdesc, "%d", fdaToi);
    //   ajouterEntree(memoire, e);
    // }
  }

  return 0;
}