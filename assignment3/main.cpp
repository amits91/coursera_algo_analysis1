#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <list>


using namespace std;
int* a = NULL;
int sz = 0;
#define START 1
#define LIST_ITERATE( iter, l ) \
    for (std::list<int>::iterator (iter) = (l)->begin(); \
            (iter) != (l)->end(); \
            ++(iter))
#define N 200
list<int> *adj;

static void parse(char* file)
{   
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    FILE* f = fopen(file, "r");
    int n;
    if (!f) {
        printf("Error: File: %s not opened\n", file);
        exit(EXIT_FAILURE);
    }
    while ((read = getline(&line, &len, f)) != -1) {
        char *endptr, *str;
        long val;
        
        str = line;
        printf("\nP: ");
        while( 1 ) {
            errno = 0;    /* To distinguish success/failure after call */
            val = strtol(str, &endptr, 10);
            if( errno ) exit(EXIT_FAILURE);
            if( !str ) break;
            if( str == endptr ) break;
            str = endptr;
            printf("\t%ld", val);
        }
        printf("\nL:\t");
        printf("%s", line);
    }
    free(line);

}

int main(int argc, char* argv[])
{
    char *file  = argv[1];

    sz    = atoi(argv[2]);
    adj   =  new list<int>[sz + 1];
    printf("File: %s, arr size: %d\n", file, sz);
    parse(file);

    delete [] adj;

    return 0;
}
/*******************************************************************/
