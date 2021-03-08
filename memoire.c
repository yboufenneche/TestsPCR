#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include "memoire.h"
// ****
void ajouterEntree(tra_t * memoire, tra_t element){
    int i = 0;
    tra_t e;
    do{
        e = memoire[i];
        if (strcmp(e.nTest, "") == 0){
            break;
        }
        i++;
    } while (1);
    memoire[i] = element;
}

// ****
void supprimerEntree(tra_t * memoire, tra_t element){
int i = 0;
    tra_t e;
    do{
        e = memoire[i];
        if (strcmp(e.nTest, element.nTest) == 0){
            break;
        }
        i++;
    } while (1);
    sprintf(e.nTest , "%s", "");
    sprintf(e.ter, "%s", "");
    memoire[i] = e;
}
// tra_t * initMemoire(int n){

// }