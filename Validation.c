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

int main (int argc, char ** argv){
    int fdr, fdw, decoupeOk;
    char nTest[255], type[255], valeur[255];
    char *msg, *rep;
    time_t now;
    t_test test;
    
    if (argc == 3){
        fdr = atoi(argv[1]);
        fdw = atoi(argv[2]);
    }
    
    msg = litLigne(fdr);
    now = time(NULL);
    
    decoupeOk = decoupe(msg, nTest, type, valeur);
    if(!decoupeOk){
        fprintf(stderr, "Erreur de découpage du message: %s\n", msg);
        exit (EXIT_FAILURE);
    }
    
    printf("Demande de validation du test [%s], validité [%s]\n", nTest, valeur);

    int fd = open("resultats.an", O_RDONLY);
    test = trouverTest (fd, nTest);
    printf("[%s] [%s] [%s]\n", test.nTest, test.date, test.res);

    unsigned long date_test = (time_t) test.date;
    now = (unsigned long) time(NULL);
    printf("Maintenant: %li\n", now);
    long diff = (long) difftime(date_test, now);
    printf("age du test: %li\n", date_test);
    char *finPtr;
    long validite = strtol(valeur, &finPtr, 10);
    char * resultat = test.res;
    if (strcmp(resultat, "negatif") == 0 && diff < validite){
        rep = message(nTest, "Reponse", "1");
    }
    else{
        rep = message(nTest, "Reponse", "0");
    }
    
    ecritLigne(fdw, rep);
}