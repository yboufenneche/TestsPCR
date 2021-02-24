#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include "lectureEcriture.h"
#include "alea.h"
#include "message.h"
int main (int argc, char *argv[]){

    int fd[2];
    int nLigne;
    int nbrLignes ;
    char nTest[255], type[255], valeur[255];
    char *demande, *reponse;
    int i;
    int fdl;
    char *c;

    // nombre de lignes (tests) dans le fichier "tests.lst"
    nbrLignes = atoi(argv[3]);

    // récupérer les descripteurs de fichiers fournis à travers la ligne de commandes
    fd[0] = atoi(argv[1]);
    fd[1] = atoi(argv[2]);

    /* récupérer un numéro de test aléatoire
       on commence par génerer un numéro de ligne alétoire,
       après on récupère le numéro de test correspondant à cette ligne dans le fichier "tests.lst"
    */
    aleainit();
    nLigne = alea(1,nbrLignes);
    fdl = open("tests.lst", O_RDONLY);
    i = 1;
    while (i < nLigne)
    {
        litLigne(fdl);
        i++;
    }
    // récupérer le numéro de test dans la ligne concernée sans prendre en compte du caractère de retour à la ligne "\n"
    strncpy (nTest, litLigne(fdl), 16);
    
    printf("Numéro de ligne généré aléatoirement: %d\n", nLigne);
    printf("Numéro de test généré aléatoirement: %s\n", nTest);

    //générer un durée de validité aléatoire entre 1000 et 2000 secondes
    sprintf(valeur, "%d", alea(1000, 2000));

    //redirection des entrées et des sorties standars vers les descripteurs de fichiers
    dup2(0, fd[0]);
    dup2(1, fd[1]);

    // générer une demande
    demande = message(nTest, "Demande", valeur);
    fprintf(stdin, "Le message à envoyer: %s\n", demande);

    //
    ecritLigne(fd[1], demande);
    printf("Le test n° %s est: %s", nTest, litLigne(fd[0]));

}
