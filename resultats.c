#include <stdio.h>
#include "resultats.h"
#include "lectureEcriture.h"
#include <string.h>

t_test trouverTest(int fd, char* nTest){
    int trouve = 0;
    char *ligne;
    char *token;
    t_test tes;
    do{
        ligne = litLigne(fd);
        token = strtok(ligne, " ");
        
        if (strcmp(nTest, token) == 0){
            tes.nTest = nTest;
            token = strtok(NULL, " ");
            tes.date = token;
            token = strtok(NULL, " ");
            tes.res = suppRetourChariot(token);
            trouve = 1;
        }
    } while (!trouve && ligne != NULL); 
    close(fd);
    return tes;  
}