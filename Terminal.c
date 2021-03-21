#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include "lectureEcriture.h"
#include "alea.h"
#include "message.h"

int main(int argc, char *argv[])
{

    int id, fdr, fdw, fdl, decoupeOk;
    int nLigne, nbrLignes, validation;
    char *nTest, type[255], valeur[255];
    char *requete, *reponse;
    int i;
    char *c;

    // nombre de lignes (tests) dans le fichier "tests.lst"
    nbrLignes = atoi(argv[3]);
    id =atoi(argv[4]);

    // récupérer les descripteurs de fichiers fournis à travers la ligne de commandes
    if (argc == 5)
    {
        fdr = atoi(argv[1]) /*open(argv[1], O_RDONLY)*/;
        fdw = atoi(argv[2]) /*open(argv[2], O_WRONLY)*/;

        fprintf(stderr, "fdr = %d\n", fdr);
        fprintf(stderr, "fdw = %d\n", fdw);
    }

    fprintf(stderr, "Ici, un terminal %d...\n", id);

    /* récupérer un numéro de test aléatoire.
       On commence par génerer un numéro de ligne alétoire,
       après on récupère le numéro de test correspondant à
       cette ligne dans le fichier "tests.lst"
    */
    aleainit();
    nLigne = alea(1, nbrLignes);
    fdl = open("tests.lst", O_RDONLY);
    i = 1;
    while (i < nLigne)
    {
        litLigne(fdl);
        i++;
    }

    // récupérer le numéro de test dans la ligne concernée sans prendre en compte du caractère de retour à la ligne "\n"
    nTest = suppRetourChariot(litLigne(fdl));
    close(fdl);

    //générer une durée de validité aléatoire entre 1000 et 2000 secondes
    sprintf(valeur, "%d", alea(1000, 2000));

    // générer une demande
    requete = message(nTest, "Demande", valeur);

    // redirection
    if(dup2(fdw, 1) < 0){
        perror("Impossible de dupliquer le descripteur du fichier");
        exit(EXIT_FAILURE);
    }

    // envoyer la demande
    if (ecritLigne(1, requete) == 0)
    {
        perror("Impossible d'envoyer la demande !\n");
        exit(EXIT_FAILURE);
    }

    // redirection
    if(dup2(fdr, 0) < 0){
        perror("Impossible de dupliquer le descripteur du fichier.\n");
        exit(EXIT_FAILURE);
    }

    // récupérer la réponse
    reponse = litLigne(0);
    fprintf(stderr, "Terminal, réponse reçu: %s \n", reponse);

    // découper la réponse
    decoupeOk = decoupe(reponse, nTest, type, valeur);

    // dire si le test est valide ou non
    if (decoupeOk)
    {
        validation = atoi(valeur);
        if (validation != 0)
        {
            fprintf(stderr, "Le test n° %s est valide. \n", nTest);
        }
        else
        {
            fprintf(stderr, "Le test n° %s n'est pas valide. \n", nTest);
        }
    }
    else
    {
        fprintf(stderr, "Erreur de découpage: %s \n", reponse);
    }

    return 0;
}
