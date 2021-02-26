typedef struct test
{
    char *nTest;
    char *date;
    char *res;
} test_t;

test_t trouverTest(int fd, char * nTest);
