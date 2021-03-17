typedef struct entree_tra
{
    char nTest[17];
    char fdesc[3];
} tra_t;

void ajouterEntree(tra_t * memoire, tra_t element);
void supprimerEntree(tra_t * memoire, tra_t element);
char * trouverEntree(tra_t * memoire, char * nTest);

// tra_t * initMemoire(int n){
// }