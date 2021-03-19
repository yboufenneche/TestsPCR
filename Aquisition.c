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

int nbTerm, tailleMem;
tra_t *memoire;
int *fdtToa, *fdaTot, fdaTov, fdvToa, fdiToa, fdaToi;

void traiterTerminal(int term);
void traiterValidation();
void traiterInterArchive();

/*
* programme principal
*/

int main(int argc, char **argv)
{

  nbTerm = atoi(argv[1]);
  tailleMem = atoi(argv[2]);
  fdtToa = malloc(nbTerm * sizeof(int));
  fdaTot = malloc(nbTerm * sizeof(int));
  memoire = calloc(tailleMem, sizeof(tra_t));

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

  //int decoupeOk;

  

  /*
      traiter les terminaux
    */

  /*
      traiter les serveurs de validation
    */
  // printf("\nTraitement du serveur Validation...\n");
  // while ((ligne = litLigne(fdvToa)) != NULL)
  // {
  //   printf("%s", ligne);
  //   decoupe(ligne, nTest, type, valeur);
  //   fd = atoi(trouverEntree(memoire, nTest));
  //   ecritLigne(fd, ligne);
  // }

  /*
      traiter les serveurs InterArchive
    */


  traiterTerminal(0);
  traiterTerminal(1);
  traiterTerminal(2);

  traiterValidation();
  traiterInterArchive();

  return 0;
}

/*
* recevoir les demandes des treminaux
*/
void traiterTerminal(int term)
{
  char *ligne/*, *mes*/;
  char nTest[17], type[8], valeur[10];
  tra_t e;
  char code[5];

  while (1)
  {
    if ((ligne = litLigne(fdtToa[term])) == NULL)
    {
      break;
    }

    printf("\nTraitement du Terminal %d...\n", term);

    printf("%s", ligne);
    //mes = suppRetourChariot(ligne);
    decoupe(ligne, nTest, type, valeur);
    strncpy(code, nTest, 4);

    sprintf(e.nTest, "%s", nTest);
    sprintf(e.fdesc, "%d", fdaTot[term]);
    ajouterEntree(memoire, e);

    if (strcmp(code, CCENTRE) == 0)
    {
      ecritLigne(fdaTov, ligne);
      //ecritLigne(fdaTov, "\n");
    }
    else
    {
      ecritLigne(fdaToi, ligne);
      //ecritLigne(fdaToi, "\n");
    }
  }
}

/*
* recevoir les réponses du serveur de validation
*/
void traiterValidation()
{
  char *ligne;
  char nTest[17], type[8], valeur[10];
  int fd;

  printf("\nTraitement du serveur Validation...\n");
  while (1)
  {
    if((ligne = litLigne(fdvToa)) == NULL){
      break;
    }
    printf("%s", ligne);
    decoupe(ligne, nTest, type, valeur);
    fd = atoi(trouverEntree(memoire, nTest));
    ecritLigne(fd, ligne);
  }
}

/*
* recevoir les demandes et les répondes du serveur InterArchive
*/
void traiterInterArchive()
{
  char *ligne, *mes;
  char nTest[17], type[8], valeur[10];
  tra_t e;
  int fd;

  printf("\nTraitement du serveur InterArchive...\n");
  while (1)
  {
    if ((ligne = litLigne(fdiToa)) == NULL)
    {
      break;
    }
    printf("%s", ligne);
    //mes = suppRetourChariot(ligne);
    decoupe(ligne, nTest, type, valeur);
    if (strcmp(valeur, "Reponse") == 0)
    {
      fd = atoi(trouverEntree(memoire, nTest));
      ecritLigne(fd, ligne);
    }
  
    else{
      sprintf(e.nTest, "%s", nTest);
      sprintf(e.fdesc, "%d", fdaToi);
      ajouterEntree(memoire, e);
    }
  }
}