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
        bool first = true;
        int node = -1;
        
        str = line;
        printf("\nP: ");
        while( 1 ) {
            errno = 0;    /* To distinguish success/failure after call */
            val = strtol(str, &endptr, 10);
            if( errno ) exit(EXIT_FAILURE);
            if( !str ) break;
            if( str == endptr ) break;
            str = endptr;
            if( first ) {
                first = false;
                node = (int)val;
            } else {
                adj[node].push_back((int)val);
            }
            printf(" %ld", val);
        }
        //printf("\nL:\t");
        //printf("%s", line);
    }
    free(line);

}

void print_graph()
{
    for (int i = 0; i <= sz; ++i) {
        if (!adj[i].empty()) {
            int v = -1;
            printf("\n%d =>", i);
            LIST_ITERATE(vi, &adj[i]) {
                v = *vi;
                printf(" %d", v);
            }
        }
    }
    printf("\n");
}

int main(int argc, char* argv[])
{
    char *file  = argv[1];

    sz    = atoi(argv[2]);
    adj   =  new list<int>[sz + 1];
    printf("File: %s, arr size: %d\n", file, sz);
    parse(file);
    print_graph();

    delete [] adj;

    return 0;
}
/*******************************************************************/
