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
    int fdt1Toa, fdaTot1, fdt2Toa, fdaTot2, fdaTov, fdvToa, fdiToa, fdaToi;

    fdt1Toa = open("t1Toa.txt", O_RDONLY);
    fdaTot1 = open("aTot1.txt", O_WRONLY);
    fdt2Toa = open("t2Toa.txt", O_RDONLY);
    fdaTot2 = open("aTot2.txt", O_WRONLY);

    fdvToa = open("vToa.txt", O_RDONLY);
    fdaTov = open("aTov.txt", O_WRONLY);
    fdiToa = open("iToa.txt", O_RDONLY);
    fdaToi = open("aToi.txt", O_WRONLY);

    char *ligne, *mes;
    char nTest[17], type[7], valeur[10];

    int decoupeOk;
    char code [5];

    /*
      traiter les terminals
    */
    while ((ligne = litLigne(fdt1Toa)) != NULL){
        mes = suppRetourChariot(ligne);
        decoupe(mes, nTest, type, valeur);
        strncpy(code, nTest, 4);

        if (strcmp(code, CCENTRE) == 0){
            ecritLigne(fdaTov, ligne);
            ecritLigne(fdaTov, "\n");
        }
        else{
            ecritLigne(fdaToi, ligne);
            ecritLigne(fdaToi, "\n");
        }
    } 

    /*
      traiter les serveurs de validation
    */
    while ((ligne = litLigne(fdvToa)) != NULL){
        ecritLigne(fdaTot1, ligne);
    } 

    /*
      traiter les serveurs InterArchive
    */
    while ((ligne = litLigne(fdiToa)) != NULL){
        ecritLigne(fdaTot1, ligne);
    } 

    return 0;
}