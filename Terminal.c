#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include "lectureEcriture.h"
#include "alea.h"
#include "message.h"

int main (int argc, char *argv[]){

    int fdr, fdw, fdl, decoupeOk;
    int nLigne, nbrLignes, validation;
    char nTest[255], type[255], valeur[255];
    char *requete, *reponse;
    int i;
    char *c;

    // nombre de lignes (tests) dans le fichier "tests.lst"
    nbrLignes = atoi(argv[3]);

    // récupérer les descripteurs de fichiers fournis à travers la ligne de commandes
    if (argc == 4){
        fdw = atoi(argv[1]);
        fdr = atoi(argv[2]);
    }

    /* récupérer un numéro de test aléatoire.
       On commence par génerer un numéro de ligne alétoire,
       après on récupère le numéro de test correspondant à cette ligne dans le fichier "tests.lst"
    */
    // aleainit();
    // nLigne = alea(1,nbrLignes);
    // fdl = open("tests.lst", O_RDONLY);
    // i = 1;
    // while (i < nLigne)
    // {
    //     litLigne(fdl);
    //     i++;
    // }
    // récupérer le numéro de test dans la ligne concernée sans prendre en compte du caractère de retour à la ligne "\n"
    //strncpy (nTest, litLigne(fdl), 17);
    //close(fdl);

    //générer un durée de validité aléatoire entre 1000 et 2000 secondes
    sprintf(valeur, "%d", alea(1000, 2000));

    // générer une demande
    requete = message("0001000000000001", "Demande", valeur);
    //printf("Demande de validation: numéro du test [%s], durée de validité [%s]\n", nTest, valeur);

    // envoyer la demande
    if (ecritLigne(5, requete) == 0){
        fprintf(stderr, "Impossible d'envoyer la demande !");
        exit(0);
    }

    // récupérer la réponse
    reponse = litLigne(fdr);
    fprintf(stderr, "Terminal, réponse reçu: %s \n", reponse);
    decoupeOk = decoupe(reponse, nTest, type, valeur);
    if(decoupeOk){
        validation = atoi(valeur);
        if (validation != 0){
            printf(" Le test n° %s est valide. \n", nTest);
        }
        else{
            printf(" Le test n° %s n'est pas valide. \n", nTest);
        }
    }
    else{
        fprintf(stderr, "Erreur de découpage: %s \n", reponse);
    }
    
    return 0;
}
