#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/wait.h>
#include "lectureEcriture.h"
#include "alea.h"
#include "message.h"

int main(int argc, char **argv)
{
    int fdt2a, fda2t, fda2v, fdv2a;
    int pid;

    char arg1[20], arg2[20];

    fdt2a = open("t2a.txt", O_RDWR);
    fda2t = open("a2t.txt", O_RDWR);
    fdv2a = open("v2a.txt", O_RDWR);
    fda2v = open("a2v.txt", O_RDWR);

    // créer un processus fils et le recouvrir avec Terminal
    switch (pid = fork())
    {
    case -1:
        /* le fork a échoué */
            fprintf(stderr, "Erreur de clonage...");
            perror("fork");
            exit(-1);
    case 0:
        /* code du fils */
        sprintf(arg1, "%d", fda2t);
        sprintf(arg2, "%d", fdt2a);
        fprintf(stderr, "Je suis un fils...\n");
        execlp("./Terminal", "Terminal", arg1, arg2, "8", NULL);
        perror ("execlp");
        exit ( EXIT_FAILURE );

    default:
        wait(NULL);
        dup2(fdt2a, 0);
        dup2(fda2t, 1);
        fprintf(stderr, "Je suis Aquisition...\n");
        fprintf(stdout, "|0001000000000001|Reponse|1|\n");
    }

    // créer un processus fils et le recouvrir avec Validation
    switch (pid = fork())
    {
    case -1:
        /* le fork a échoué */
            fprintf(stderr, "Erreur de clonage...");
            perror("fork");
            exit(-1);
    case 0:
        /* code du fils */
        sprintf(arg1, "%d", fda2v);
        sprintf(arg2, "%d", fdv2a);
        fprintf(stderr, "Je suis un fils qui sera recouvert par Validation...\n");
        execlp("./Validation", "Validation", arg1, arg2, NULL);
        perror ("execlp");
        exit (EXIT_FAILURE);

    default:
        wait(NULL);
        dup2(fdv2a, 0);
        dup2(fda2v, 1);
        fprintf(stderr, "Je suis Aquisition -- Validation...\n");
        //fprintf(stdout, "|0001000000000001|Demande|35000|\n");
    }

    return 0;
}