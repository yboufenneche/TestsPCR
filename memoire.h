typedef struct entree_tra
{
    char nTest[17];
    int fdesc;
} tra_t;

void ajouterEntree(tra_t * memoire, tra_t element);
void supprimerEntree(tra_t * memoire, char * nTest);
int trouverEntree(tra_t * memoire, char * nTest);
void afficherMemoire(tra_t * memoire, int taille);

// tra_t * initMemoire(int n){
// }