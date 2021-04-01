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
#include "InterArchive.h"

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
    noms_centres = malloc(nbCentres * sizeof(char *));
    codes_centres = malloc(nbCentres * sizeof(char *));
    fichiers_res = malloc(nbCentres * sizeof(char *));
    nb_terms = malloc(nbCentres * sizeof(int));

    fd_conf = open(conf, O_RDONLY);
    annuaire = suppRetourChariot(litLigne(fd_conf));
    printf("Annuaire des centres d'archivage: %s\n", annuaire);
    fd_an = open(annuaire, O_RDONLY);

    char *fichier;

    for (int i = 0; i < nbCentres; i++)
    {
        a = malloc(sizeof(char *));
        b = malloc(sizeof(char *));
        ligne = litLigne(fd_an);
        fichier = suppRetourChariot(litLigne(fd_conf));
        decoupe2(ligne, a, b);
        *(noms_centres + i) = a;
        *(codes_centres + i) = b;
        *(fichiers_res + i) = fichier;
        // printf("%s %s %s\n", *(noms_centres + i), *(codes_centres + i), *(fichiers_res + i));
    }

    liaisonsAqui = (liaison_t *)malloc(nbCentres * sizeof(liaison_t));
    char *nbt;

    for (int i = 0; i < nbCentres; i++)
    {
        nbt = suppRetourChariot(litLigne(fd_conf));
        *(nb_terms + i) = atoi(nbt);
        // printf("%s %s %s %d\n", *(noms_centres + i), *(codes_centres + i), *(fichiers_res + i), *(nb_terms + i));
        pipe((liaisonsAqui + i)->pipeReceive);
        pipe((liaisonsAqui + i)->pipeSend);
        // printf("Aquisition %d\n", i);
        // printf("pipeReceive: [%d, %d]\n", (liaisonsAqui + i)->pipeReceive[0], (liaisonsAqui + i)->pipeReceive[1]);
        // printf("pipeSend:    [%d, %d]\n", (liaisonsAqui + i)->pipeSend[0], (liaisonsAqui + i)->pipeSend[1]);
    }

    pid_t aquisition;
    char nom[20], code[20], fic[20], nt[20], tm[4]; /*, fd3[5], nAqui[5]*/
    ;

    // création des processus Aquisition avec fork et execlp
    for (int i = 0; i < 2; i++)
    {
        printf("%d\n", i);
        switch (aquisition = fork())
        {
        case -1:
            /* le fork a échoué */
            perror("fork");
            exit(-1);
        case 0:
            sprintf(nom, "%s", *(noms_centres + i));
            sprintf(code, "%s", *(codes_centres + i));
            sprintf(fic, "%s", *(fichiers_res + i));
            sprintf(nt, "%d", *(nb_terms + i));
            sprintf(tm, "%d", tailleMem);
            // printf("Rec. Aquisition %d: fd1 = %s, fd2 = %s\n", i, fd1, fd2);

            printf("[Rec. Aquisition %d]: %s %s %s %s %s\n", i, nom, code, fic, nt, tm);
            execlp("/usr/bin/xterm", "xterm", "-e", "./Aquisition", nom, code, fic, nt, tm, NULL);
            break;
        default:
            break;
        }
    }

    sem_init(&mutex, 0, 1);
    sem_init(&vide, 0, tailleMem);

    pthread_t thread_aqui[nbCentres];

    // threads Aquisitions
    for (int i = 0; i < nbCentres; i++)
    {
        pthread_create(&thread_aqui[i], NULL, traiterAquisition, (void *)(intptr_t)i);
    }

    for (int i = 0; i < nbCentres; i++)
    {
        pthread_join(thread_aqui[i], NULL);
    }

    return 0;
}

/********************************************************/
/*******   Fonction à éxecuter via les threads   *******/
/********************************************************/

/*
* recevoir les messages des serveurs Aquisition
*/
void *traiterAquisition(void *arg)
{
    char *ligne;
    char nTest[17], type[8], valeur[10];
    tra_t e;
    char code[5];
    int aqui = (intptr_t)arg;
    int fdesc, num_centre;

    while (1)
    {
        ligne = litLigne((liaisonsAqui + aqui)->pipeReceive[0]);

        // printf("\nTraitement du Terminal %d...\n", term);
        printf("[InterArchive], message reçu [Aquisition %d]: %s", aqui, ligne);
        decoupe(ligne, nTest, type, valeur);
        strncpy(code, nTest, 4);

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
            e.fdesc = (liaisonsAqui + aqui)->pipeSend[1];
            sem_wait(&vide);
            sem_wait(&mutex);
            ajouterEntree(memoire, e);
            // afficherMemoire(memoire, tailleMem);
            sem_post(&mutex);
            num_centre = trouverCentre(code, codes_centres, nbCentres);
            ecritLigne((liaisonsAqui + num_centre)->pipeSend[1], ligne);
        }
    }
    return (void *)0;
}

/*
* Trouver l'emplacement d'un code de centre dans le tableau contenant tous les codes
*/
int trouverCentre(char *code, char ** codes_centres, int taille){
    int pos = -1;
    for (int i = 0; i < taille; i++){
        if(strcmp(code, *(codes_centres + i)) == 0){
            pos = i;
            break;
        }
    }
    return pos;
}