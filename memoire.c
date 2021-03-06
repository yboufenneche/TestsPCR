#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include "memoire.h"

// **** ajouter une entrée dans la table de routage
void ajouterEntree(tra_t * memoire, tra_t element){
    int i = 0;
    tra_t e;
    do{
        e = memoire[i];
        if (strcmp(e.nTest, "") == 0){
            break;
        }
        i++;
    } while (1);
    memoire[i] = element;
}

// **** supprimer une entrée de la table de routage
void supprimerEntree(tra_t * memoire, char * nTest){
    int i = 0;
    tra_t e;
    do{
        e = memoire[i];
        if (strcmp(e.nTest, nTest) == 0){
            break;
        }
        i++;
    } while (1);
    sprintf(e.nTest , "%s", "");
    e.fdesc = -1;
    memoire[i] = e;
}

// **** trouver le numéro du terminal 
int trouverEntree(tra_t * memoire, char * nTest){
    int i = 0;
    tra_t e;
    do{
        e = memoire[i];
        if (strcmp(e.nTest, nTest) == 0){
            break;
        }
        i++;
    } while (1);
    return memoire[i].fdesc;
}

// **** afficher le contenu de la mémoire du serveur
void afficherMemoire(tra_t * memoire, int taille){
    for (int i = 0; i < taille; i++)
    {
        printf ("[%s] [%d]\n", (memoire+i)->nTest, (memoire+i)->fdesc);
    }
}