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
    char *msg;
    time_t now;
    t_test test;
    
    if (argc == 3){
        fdr = atoi(argv[1]);
        fdw = atoi(argv[2]);
    }
    
    // msg = litLigne(fdr);
    // now = time(NULL);
    
    // decoupeOk = decoupe(msg, nTest, type, valeur);
    // if(!decoupeOk){
    //     fprintf(stderr, "Erreur de découpage du message: %s\n", msg);
    //     exit (EXIT_FAILURE);
    // }
    
    // printf("Demande de validation du test [%s], validité [%s]\n", nTest, valeur);
    // fprintf(stdout, "%lu\n", (unsigned long)time(NULL));
    int fd = open("resultats.an", O_RDONLY);
    test = trouverTest (fd, "0001000000000011");
    printf("%s %s %s\n", test.nTest, test.date, test.res);
    close(fd);
    printf("%s\n %s\n %s\n", test.nTest, test.date, test.res);


}