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

char **codes_centres, **noms_centres, **fichiers_res, *conf, *annuaire, *ligne, *a, *b;
int nbCentres, *nb_terms, tailleMem, fd_conf, fd_an;
tra_t *memoire;

liaison_t *liaisonsAqui;

sem_t vide, mutex;

void *traiterAquisition(void *arg);

/*
* programme principal
*/

int main(int argc, char **argv)
{
    conf = argv[1];
    tailleMem = atoi(argv[2]);

    memoire = (tra_t *)malloc(tailleMem * sizeof(tra_t));

    nbCentres = 2;
    noms_centres  = malloc(nbCentres * sizeof(char*));
    codes_centres = malloc(nbCentres * sizeof(char*));
    fichiers_res  = malloc(nbCentres * sizeof(char*));
    nb_terms = malloc(nbCentres * sizeof(int));

    fd_conf = open(conf, O_RDONLY);
    annuaire = suppRetourChariot(litLigne(fd_conf));
    printf("%s\n", annuaire);
    fd_an = open(annuaire, O_RDONLY);
    a = malloc(sizeof(char *));
    b = malloc(sizeof(char *));

    char* fichier;

    for (int i = 0; i < nbCentres; i++)
    {
        ligne = litLigne(fd_an);
        fichier = suppRetourChariot(litLigne(fd_conf));
        decoupe2(ligne, a, b);
        *(noms_centres + i) = a;
        *(codes_centres + i) = b;
        *(fichiers_res + i) = fichier;
        printf("%s --> %s\n", *(noms_centres + i), *(codes_centres + i));
        printf("Fichier resultats [%d]: %s\n", i, *(fichiers_res + i));
    }
    

    liaisonsAqui = (liaison_t *)malloc(nbCentres * sizeof(liaison_t));
    char *nbt;

    for (int i = 0; i < nbCentres; i++)
    {
        nbt = suppRetourChariot(litLigne(fd_conf));
        *(nb_terms + i) = atoi(nbt);
        printf("nbt[%d]: %d\n", i, *(nb_terms + i));
        pipe((liaisonsAqui + i)->pipeReceive);
        pipe((liaisonsAqui + i)->pipeSend);
        printf("Aquisition %d\n", i);
        printf("pipeReceive: [%d, %d]\n", (liaisonsAqui + i)->pipeReceive[0], (liaisonsAqui + i)->pipeReceive[1]);
        printf("pipeSend:    [%d, %d]\n", (liaisonsAqui + i)->pipeSend[0], (liaisonsAqui + i)->pipeSend[1]);
    }

    pid_t aquisition;
    char fd1[5], fd2[5] /*, fd3[5]*/, nAqui[5];

    // // création des processus InterArchive avec fork et execlp
    // for (int i = 0; i < nbCentres; i++)
    // {
    //     switch (aquisition = fork())
    //     {
    //     case -1:
    //         /* le fork a échoué */
    //         perror("fork");
    //         exit(-1);
    //     case 0:
    //         sprintf(fd1, "%d", (liaisonsAqui + i)->pipeSend[0]);
    //         sprintf(fd2, "%d", (liaisonsAqui + i)->pipeReceive[1]);
    //         sprintf(nterm, "%d", i);
    //         printf("Rec. Aquisition %d: fd1 = %s, fd2 = %s\n", i, fd1, fd2);
    //         execlp("/usr/bin/xterm", "xterm", "-e", "./Terminal", fd1, fd2, "20", nterm, NULL);
    //         break;
    //     default:
    //         break;
    //     }
    // }

    // sem_init(&mutex, 0, 1);
    // sem_init(&vide, 0, tailleMem);

    // pthread_t thread_aqui[nbCentres];

    // // threads Aquisitions
    // for (int i = 0; i < nbCentres; i++)
    // {
    //     pthread_create(&thread_aqui[i], NULL, traiterAquisition, (void *)(intptr_t)i);
    // }

    // for (int i = 0; i < nbCentres; i++)
    // {
    //     pthread_join(thread_aqui[i], NULL);
    // }

    return 0;
}

/********************************************************/
/*******   Fonction à éxecuter via les threads   *******/
/********************************************************/

/*
* recevoir les messages des serveurs Aquisition
*/
// void *traiterAquisition(void *arg)
// {
//     char *ligne;
//     char nTest[17], type[8], valeur[10];
//     tra_t e;
//     char code[5];
//     int term = (intptr_t)arg;

//     while (1)
//     {
//         ligne = litLigne((liaisonsAqui + term)->pipeReceive[0]);

//         // printf("\nTraitement du Terminal %d...\n", term);
//         printf("[InterArchive], demande reçue [Aquisition %d]: %s", term, ligne);
//         decoupe(ligne, nTest, type, valeur);
//         strncpy(code, nTest, 4);

//         sprintf(e.nTest, "%s", nTest);
//         e.fdesc = (liaisonsAqui + term)->pipeSend[1];
//         // printf("%d\n", e.fdesc);
//         sem_wait(&vide);
//         sem_wait(&mutex);
//         ajouterEntree(memoire, e);
//         afficherMemoire(memoire, tailleMem);
//         sem_post(&mutex);
//         if (strcmp(code, code_centre) == 0)
//         {
//             ecritLigne(liaisonValid.pipeSend[1], ligne);
//         }
//         else
//         {
//             ecritLigne(liaisonInter.pipeSend[1], ligne);
//         }
//     }
//     return (void *)0;
// }