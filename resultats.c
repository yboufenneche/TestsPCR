#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "resultats.h"
#include "lectureEcriture.h"
#include <string.h>

test_t trouverTest(int fd, char* nTest){
    int trouve = 0;
    char *ligne;
    char *token;
    test_t tes;
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