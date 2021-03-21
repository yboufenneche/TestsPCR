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
#include "Aquisition.h"

#define CCENTRE "0001"
// #define R 0;
// #define W 1;

int nbTerm, tailleMem;
tra_t *memoire;

liaison_t *liaisonsTerm;
liaison_t liaisonValid;
liaison_t liaisonInter;

// int *fdtToa, *fdaTot, fdaTov, fdvToa, fdiToa, fdaToi;

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
  // fdtToa = malloc(nbTerm * sizeof(int));
  // fdaTot = malloc(nbTerm * sizeof(int));
  memoire = (tra_t *)malloc(tailleMem * sizeof(tra_t));
  liaisonsTerm = (liaison_t *)malloc(nbTerm * sizeof(liaison_t));

  for (int i = 0; i < nbTerm; i++)
  {
    pipe((liaisonsTerm + i)->pipeReceive);
    pipe((liaisonsTerm + i)->pipeSend);
  }

  pipe(liaisonValid.pipeSend);
  pipe(liaisonValid.pipeReceive);

  pipe(liaisonInter.pipeSend);
  pipe(liaisonInter.pipeReceive);

  pid_t terminal;
  char t1[5], t2[5];

  for (int i = 0; i < nbTerm; i++)
  {
    switch (terminal = fork())
    {
    case -1:
      /* le fork a échoué */
      perror("fork");
      exit(-1);
    case 0:
      // close((liaisonsTerm + i)->pipeReceive[1]);
      // close((liaisonsTerm + i)->pipeSend[0]);
      sprintf(t1, "%d", (liaisonsTerm + i)->pipeSend[0]);
      sprintf(t2, "%d", (liaisonsTerm + i)->pipeReceive[1]);
      execlp("xterm -e Terminal","Terminal", t1, t2, "20", "1", NULL);
    //default:
      // close((liaisonsTerm + i)->pipeReceive[0]);
      // close((liaisonsTerm + i)->pipeSend[1]);
    }
  }

  pid_t validation;
  switch (validation = fork())
  {
  case -1:
    /* le fork a échoué */
    perror("fork");
    exit(-1);
  case 0:
    close(liaisonValid.pipeReceive[1]);
    close(liaisonValid.pipeSend[0]);
    sprintf(t1, "%d", liaisonValid.pipeReceive[0]);
    sprintf(t2, "%d", liaisonValid.pipeReceive[1]);
    execlp("./Validation","Validation", t1, t2, NULL);
  default:
    close(liaisonValid.pipeReceive[0]);
    close(liaisonValid.pipeSend[1]);
  }

  sem_init(&mutex, 0, 1);
  sem_init(&vide, 0, tailleMem);

  pthread_t thread_term[nbTerm];
  pthread_t thread_valid;
  pthread_t thread_inter;

  // threads Terminaux
  for (int i = 0; i < nbTerm; i++)
  {
    pthread_create(&thread_term[i], NULL, traiterTerminal, (void *)(intptr_t)i);
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
  int term = (intptr_t)arg;

  while (1)
  {
    ligne = litLigne((liaisonsTerm + term)->pipeReceive[0]);

    // printf("\nTraitement du Terminal %d...\n", term);

    printf("[Terminal %d] %s", term, ligne);
    decoupe(ligne, nTest, type, valeur);
    strncpy(code, nTest, 4);

    sprintf(e.nTest, "%s", nTest);
    e.fdesc = (liaisonsTerm + term)->pipeSend[1];
    sem_wait(&vide);
    sem_wait(&mutex);
    ajouterEntree(memoire, e);
    // afficherMemoire(memoire, tailleMem);
    sem_post(&mutex);
    if (strcmp(code, CCENTRE) == 0)
    {
      ecritLigne(liaisonValid.pipeSend[1], ligne);
    }
    else
    {
      ecritLigne(liaisonInter.pipeSend[1], ligne);
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
  int fdesc;

  // printf("\nTraitement du serveur Validation...\n");
  while (1)
  {
    ligne = litLigne(liaisonValid.pipeReceive[1]);
    printf("[Validation] %s", ligne);
    decoupe(ligne, nTest, type, valeur);
    sem_wait(&mutex);
    fdesc = trouverEntree(memoire, nTest);
    ecritLigne(fdesc, ligne);
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
  int fdesc;

  printf("\nTraitement du serveur InterArchive...\n");
  while (1)
  {
    ligne = litLigne(liaisonInter.pipeReceive[0]);
    printf("[InterArchive] %s", ligne);
    decoupe(ligne, nTest, type, valeur);

    if (strcmp(type, "Reponse") == 0)
    {
      sem_wait(&mutex);
      fdesc = trouverEntree(memoire, nTest);
      ecritLigne(fdesc, ligne);
      supprimerEntree(memoire, nTest);
      // afficherMemoire(memoire, tailleMem);
      sem_post(&mutex);
      sem_post(&vide);
    }

    else
    {
      sprintf(e.nTest, "%s", nTest);
      e.fdesc = liaisonInter.pipeSend[1];
      sem_wait(&vide);
      sem_wait(&mutex);
      ajouterEntree(memoire, e);
      // afficherMemoire(memoire, tailleMem);
      sem_post(&mutex);
    }
  }
  return (void *)0;
}