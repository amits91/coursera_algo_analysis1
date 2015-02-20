#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <list>
#include <vector>
#include <map>
#include <time.h>
#include <algorithm>
using namespace std;


int sz = 0;
int m = 0;

typedef vector<int> vertexT;
typedef vector<vertexT> graphT;
graphT oG;
#define VECTOR_ITERATE( iter, l ) \
    for (std::vector<int>::iterator (iter) = (l)->begin(); \
            (iter) != (l)->end(); \
            ++(iter))
#define GRAPH_ITERATE( iter, l ) \
    for (std::vector<vertexT>::iterator (iter) = (l)->begin(); \
            (iter) != (l)->end(); \
            ++(iter))
#define EDGE_ITERATE( iter, l ) \
    for (std::vector<int>::iterator (iter) = (l)->begin() + 1; \
            (iter) != (l)->end(); \
            ++(iter))

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
        //printf("\nP: ");
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
                oG[node].push_back(node);
            } else {
                int v = (int)val;
                oG[node].push_back(v);
                ++m;
            }
            //printf(" %ld", val);
        }
        //printf("\nL:\t");
        //printf("%s", line);
    }
    free(line);

}

void print_vector( vector <int> l )
{
    bool first= true;
    VECTOR_ITERATE(vi, &l) {
        int e = *vi;
        if (first) {
            first = false;
            printf("%d =>", e);
        } else {
            printf(" %d", e);
        }
    }
    printf("\n");
}

void print_gi( vector<vertexT> g)
{
    for (int i = 0; i < g.size(); ++i) {
        if (!g[i].empty()) {
            print_vector(g[i]);
        }
    }
    printf("\n");
}

int main(int argc, char* argv[])
{
    char *file  = argv[1];

    sz      = atoi(argv[2]);
    oG.resize(sz + 1);
    parse(file);
    return 0;
}
