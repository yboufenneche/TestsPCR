#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include "lectureEcriture.h"
#include "alea.h"
#include "message.h"
#include "resultats.h"

int main(int argc, char **argv)
{
    int fdr, fdw, decoupeOk;
    char nTest[255], type[255], valeur[255];
    char *msg, *rep;
    time_t now;
    test_t test;

    fprintf(stderr, "Ici, Validation...\n");

    // récupérer les descripteurs de fichiers fournis à travers la ligne de commandes
    if (argc == 3)
    {
        fdr = atoi(argv[1]);
        fdw = atoi(argv[2]);

        fprintf(stderr, "Validation: fdr = %d, fdw = %d\n", fdr, fdw);
    }

    // redirection
    if(dup2(fdr, 0) < 0){
        perror("Impossible de dupliquer le descripteur du fichier");
        exit(EXIT_FAILURE);
    }

    // récupérer la demande de validation (le message)
    msg = litLigne(0);

    now = time(NULL);

    // tenter de découper le message
    decoupeOk = decoupe(msg, nTest, type, valeur);
    if (!decoupeOk)
    {
        fprintf(stderr, "Erreur de découpage du message: %s\n", msg);
        exit(EXIT_FAILURE);
    }

    // chercher le test dans l'annuaire "resultats.an"
    int fd = open("resultats.ar", O_RDONLY);
    test = trouverTest(fd, nTest);
    fprintf(stderr, "Test trouvé: [%s] [%s] [%s]\n", test.nTest, test.date, test.res);

    // convertir la date du test du chaîne à long
    long date_test = (strtol(test.date, NULL, 10));

    // calculer l'age du test
    now = (long)time(NULL);
    long diff = (long)difftime(now, date_test);

    // convertir la durée de validité du test du chaîne à long
    long validite = (strtol(valeur, NULL, 10));
    char *resultat = test.res;

    // redirection
    if(dup2(fdw, 1) < 0){
        perror("Impossible de dupliquer le descripteur du fichier");
        exit(EXIT_FAILURE);
    }

    // préparer la réponse
    if (strcmp(resultat, "negatif") == 0 && diff < validite)
    {
        rep = message(nTest, "Reponse", "1");
    }
    else
    {
        rep = message(nTest, "Reponse", "0");
    }

    // écrire la réponse dans le fichier
    ecritLigne(1, rep);

    return 0;
}