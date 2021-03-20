#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/wait.h>
#include <pthread.h>
#include <semaphore.h>
#include "lectureEcriture.h"
#include "alea.h"
#include "message.h"
#include "memoire.h"

#define CCENTRE "0001"

int nbTerm, tailleMem;
tra_t *memoire;
int *fdtToa, *fdaTot, fdaTov, fdvToa, fdiToa, fdaToi;

sem_t vide, mutex;

void *traiterTerminal(void *arg);
void *traiterValidation();
void *traiterInterArchive();

/*
* programme principal
*/

int main(int argc, char **argv)
{

  nbTerm = atoi(argv[1]);
  tailleMem = atoi(argv[2]);
  fdtToa = malloc(nbTerm * sizeof(int));
  fdaTot = malloc(nbTerm * sizeof(int));
  memoire = (tra_t*) malloc(tailleMem * sizeof(tra_t));

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

  sem_init(&mutex, 0, 1);
  sem_init(&vide, 0, tailleMem);

  pthread_t thread_term[nbTerm];
  pthread_t thread_valid;
  pthread_t thread_inter;

  // threads Terminaux
  for (int i = 0; i < nbTerm; i++)
  {
    pthread_create(&thread_term[i], NULL, traiterTerminal, (void *) (intptr_t) i);
  }

  for (int i = 0; i < nbTerm; i++)
  {
    pthread_join(thread_term[i], NULL);
  }

  // thread Validation et thread Interarchive
  pthread_create(&thread_valid, NULL, traiterValidation, NULL);
  pthread_create(&thread_inter, NULL, traiterInterArchive, NULL);
  
  pthread_join(thread_valid, NULL);
  pthread_join(thread_inter, NULL);

  return 0;
}

/********************************************************/
/*******   Fonctions à éxecuter via les threads   *******/
/********************************************************/

/*
* recevoir les demandes des treminaux
*/
void *traiterTerminal(void *arg)
{
  char *ligne;
  char nTest[17], type[8], valeur[10];
  tra_t e;
  char code[5];
  int term = (intptr_t) arg;

  while (1)
  {
    if ((ligne = litLigne(fdtToa[term])) == NULL)
    {
      break;
    }

    // printf("\nTraitement du Terminal %d...\n", term);

    printf("[Terminal %d] %s", term, ligne);
    decoupe(ligne, nTest, type, valeur);
    strncpy(code, nTest, 4);

    sprintf(e.nTest, "%s", nTest);
    sprintf(e.fdesc, "%d", fdaTot[term]);
    sem_wait(&vide);
    sem_wait(&mutex);
    ajouterEntree(memoire, e);
    // afficherMemoire(memoire, tailleMem);
    sem_post(&mutex);
    if (strcmp(code, CCENTRE) == 0)
    {
      ecritLigne(fdaTov, ligne);
    }
    else
    {
      ecritLigne(fdaToi, ligne);
    }
  }
  return (void *)0;
}

/*
* recevoir les réponses du serveur de validation
*/
void *traiterValidation()
{
  char *ligne;
  char nTest[17], type[8], valeur[10];
  int fd;

  // printf("\nTraitement du serveur Validation...\n");
  while (1)
  {
    if((ligne = litLigne(fdvToa)) == NULL){
      break;
    }
    printf("[Validation] %s", ligne);
    decoupe(ligne, nTest, type, valeur);
    sem_wait(&mutex);
    fd = atoi(trouverEntree(memoire, nTest));
    ecritLigne(fd, ligne);
    supprimerEntree(memoire, nTest);
    // afficherMemoire(memoire, tailleMem);
    sem_post(&mutex);
    sem_post(&vide);
  }
  return (void *)0;
}

/*
* recevoir les demandes et les répondes du serveur InterArchive
*/
void *traiterInterArchive()
{
  char *ligne;
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
    printf("[InterArchive] %s", ligne);
    decoupe(ligne, nTest, type, valeur);

    if (strcmp(type, "Reponse") == 0)
    {
      sem_wait(&mutex);
      fd = atoi(trouverEntree(memoire, nTest));
      ecritLigne(fd, ligne);
      supprimerEntree(memoire, nTest);
      // afficherMemoire(memoire, tailleMem);
      sem_post(&mutex);
      sem_post(&vide);
    }
  
    else{
      sprintf(e.nTest, "%s", nTest);
      sprintf(e.fdesc, "%d", fdaToi);
      sem_wait(&vide);
      sem_wait(&mutex);
      ajouterEntree(memoire, e);
      // afficherMemoire(memoire, tailleMem);
      sem_post(&mutex);
    }
  }
  return (void *)0;
}