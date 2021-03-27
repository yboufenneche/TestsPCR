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

// #define R 0;
// #define W 1;

char *code_centre, *nom_centre, *fichier_resultats;
int nbTerm, tailleMem, fd_res;
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

  nom_centre = argv[1];
  code_centre = argv[2];
  fichier_resultats = argv[3];
  nbTerm = atoi(argv[4]);
  tailleMem = atoi(argv[5]);
  memoire = (tra_t *)malloc(tailleMem * sizeof(tra_t));
  liaisonsTerm = (liaison_t *)malloc(nbTerm * sizeof(liaison_t));
  // fd_res = open(fichier_resultats, O_RDONLY);
  // printf("fd_res: %d\n", fd_res);

  for (int i = 0; i < nbTerm; i++)
  {
    pipe((liaisonsTerm + i)->pipeReceive);
    pipe((liaisonsTerm + i)->pipeSend);
    printf("Terminal %d\n", i);
    printf("pipeReceive: [%d, %d]\n", (liaisonsTerm + i)->pipeReceive[0], (liaisonsTerm + i)->pipeReceive[1]);
    printf("pipeSend:    [%d, %d]\n", (liaisonsTerm + i)->pipeSend[0], (liaisonsTerm + i)->pipeSend[1]);
  }

  pipe(liaisonValid.pipeSend);
  pipe(liaisonValid.pipeReceive);
  printf("Validation\n");
  printf("pipeReceive: [%d, %d]\n", liaisonValid.pipeReceive[0], liaisonValid.pipeReceive[1]);
  printf("pipeSend:    [%d, %d]\n", liaisonValid.pipeSend[0], liaisonValid.pipeSend[1]);
  

  pipe(liaisonInter.pipeSend);
  pipe(liaisonInter.pipeReceive);
  printf("InterArchive\n");
  printf("pipeReceive: [%d, %d]\n", liaisonInter.pipeReceive[0], liaisonInter.pipeReceive[1]);
  printf("pipeSend:    [%d, %d]\n", liaisonInter.pipeSend[0], liaisonInter.pipeSend[1]);
  
  pid_t terminal;
  char fd1[5], fd2[5]/*, fd3[5]*/, nterm[5];

  // création des processus terminaux avec fork et execlp
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
      sprintf(fd1, "%d", (liaisonsTerm + i)->pipeSend[0]);
      sprintf(fd2, "%d", (liaisonsTerm + i)->pipeReceive[1]);
      sprintf(nterm, "%d", i);
      printf("Rec. Terminal %d: fd1 = %s, fd2 = %s\n", i, fd1, fd2);
      execlp("/usr/bin/xterm", "xterm", "-e", "./Terminal", fd1, fd2, "20", nterm, NULL);
      break;
    default:
      // close((liaisonsTerm + i)->pipeReceive[0]);
      // close((liaisonsTerm + i)->pipeSend[1]);
      break;
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
    // close(liaisonValid.pipeReceive[1]);
    // close(liaisonValid.pipeSend[0]);
    sprintf(fd1, "%d", liaisonValid.pipeSend[0]);
    sprintf(fd2, "%d", liaisonValid.pipeReceive[1]);
    // sprintf(fd3, "%d", fd_res);
    printf("Rec. Validation: fd1 = %s, fd2 = %s, fichier res = %s\n", fd1, fd2, fichier_resultats);
    // execlp("./Validation", "./Validation", fd1, fd2, fd3, NULL);
    execlp("/usr/bin/xterm", "xterm", "-e", "./Validation", fd1, fd2, fichier_resultats, NULL);
    break;
  default:
    // close(liaisonValid.pipeReceive[0]);
    // close(liaisonValid.pipeSend[1]);
    break;
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

  // thread Validation et thread Interarchive
  pthread_create(&thread_valid, NULL, traiterValidation, NULL);
  pthread_create(&thread_inter, NULL, traiterInterArchive, NULL);

  for (int i = 0; i < nbTerm; i++)
  {
    pthread_join(thread_term[i], NULL);
  }

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
    printf("[Aquisition], demande reçue [Terminal %d]: %s", term, ligne);
    decoupe(ligne, nTest, type, valeur);
    strncpy(code, nTest, 4);

    sprintf(e.nTest, "%s", nTest);
    e.fdesc = (liaisonsTerm + term)->pipeSend[1];
    // printf("%d\n", e.fdesc);
    sem_wait(&vide);
    sem_wait(&mutex);
    ajouterEntree(memoire, e);
    afficherMemoire(memoire, tailleMem);
    sem_post(&mutex);
    if (strcmp(code, code_centre) == 0)
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
    ligne = litLigne(liaisonValid.pipeReceive[0]);
    printf("[Aquisition], réponse de [Validation] %s", ligne);
    decoupe(ligne, nTest, type, valeur);
    sem_wait(&mutex);
    fdesc = trouverEntree(memoire, nTest);
    printf("%d\n", fdesc);
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
    printf("[Aquisition], message reçu [InterArchive]: %s", ligne);
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