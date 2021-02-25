typedef struct test
{
    char *nTest;
    char *date;
    char *res;
} t_test;

t_test trouverTest(int fd, char * nTest);
