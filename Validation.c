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
    int fdr, fdw, fd_res, decoupeOk;
    char nTest[17], type[8], valeur[10];
    char *msg, *rep, *fich_res;
    time_t now;
    test_t test;

    // récupérer les descripteurs de fichiers fournis à travers la ligne de commandes
    if (argc == 4)
    {
        fdr = atoi(argv[1]);
        fdw = atoi(argv[2]);
        fich_res = argv[3];

        fprintf(stderr, "Ici, Validation: [%d, %d, %s]\n", fdr, fdw, fich_res);
    }

    // redirection
    // if(dup2(fdr, 0) < 0){
    //     perror("Impossible de dupliquer le descripteur du fichier");
    //     exit(EXIT_FAILURE);
    // }
    while (1)
    {
        fprintf(stderr, "Attente d'une demande de validation...\n");
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
        fd_res = open(fich_res, O_RDONLY);
        test = trouverTest(fd_res, nTest);
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
        // if(dup2(fdw, 1) < 0){
        //     perror("Impossible de dupliquer le descripteur du fichier");
        //     exit(EXIT_FAILURE);
        // }

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

    return 0;
}