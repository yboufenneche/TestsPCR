typedef struct liaison
{
    int pipeSend[2];
    int pipeReceive[2];
} liaison_t;

int trouverCentre(char *code, char ** codes_centres, int taille);