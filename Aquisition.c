#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/wait.h>
#include "lectureEcriture.h"
#include "alea.h"
#include "message.h"

#define CCENTRE "0001"

int main(int argc, char **argv)
{
    int fdt2a, fda2t, fda2v, fdv2a, fdi2a, fda2i;

    fdt2a = open("t2a.txt", O_RDONLY);
    fda2t = open("a2t.txt", O_WRONLY);
    fdv2a = open("v2a.txt", O_RDONLY);
    fda2v = open("a2v.txt", O_WRONLY);
    fdi2a = open("i2a.txt", O_RDONLY);
    fda2i = open("a2i.txt", O_WRONLY);

    char *ligne, *mes;
    char nTest[17], type[7], valeur[10];

    int decoupeOk;
    char code [5];

    /*
      traiter les terminals
    */
    while ((ligne = litLigne(fdt2a)) != NULL){
        mes = suppRetourChariot(ligne);
        decoupe(mes, nTest, type, valeur);
        strncpy(code, nTest, 4);

        if (strcmp(code, CCENTRE) == 0){
            ecritLigne(fda2v, ligne);
            ecritLigne(fda2v, "\n");
        }
        else{
            ecritLigne(fda2i, ligne);
            ecritLigne(fda2i, "\n");
        }
    } 

    /*
      traiter les serveurs de validation
    */
    while ((ligne = litLigne(fdv2a)) != NULL){
        ecritLigne(fda2t, ligne);
    } 

    /*
      traiter les serveurs InterArchive
    */
    while ((ligne = litLigne(fdi2a)) != NULL){
        ecritLigne(fda2t, ligne);
    } 

    return 0;
}