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

    if (argc == 3)
    {
        fdr = atoi(argv[1]);
        fdw = atoi(argv[2]);
    }

    // récupérer la demande de validation (le message)
    msg = litLigne(fdr);
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
    printf("Test trouvé: [%s] [%s] [%s]\n", test.nTest, test.date, test.res);

    // convertir la date du test du chaîne à long
    char *finPtr;
    long date_test = (strtol(test.date, &finPtr, 10));

    // calculer l'age du test
    now = (long)time(NULL);
    long diff = (long)difftime(now, date_test);

    // convertir la durée de validité du test du chaîne à long
    long validite = (strtol(valeur, &finPtr, 10));
    char *resultat = test.res;

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
    ecritLigne(fdw, rep);
}