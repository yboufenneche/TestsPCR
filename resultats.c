#include <stdio.h>
#include "resultats.h"
#include "lectureEcriture.h"
#include <string.h>

t_test trouverTest(int fd, char* nTest){
    int trouve = 0;
    char *ligne;
    char *p;
    t_test tes;
    do{
        ligne = litLigne(fd);
        p = strtok(ligne, " ");
        if (strcmp(nTest, p) == 0){
            tes.nTest = nTest;
            tes.date  = strtok(ligne, " ");;
            tes.res   = strtok(ligne, " ");;
            trouve = 1;
        }
    } while (!trouve && ligne != NULL); 
    return tes;  
}