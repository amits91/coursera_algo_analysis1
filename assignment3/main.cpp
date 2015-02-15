#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <list>
using namespace std;


int sz = 0;
list<int> *adj;
#define LIST_ITERATE( iter, l ) \
    for (std::list<int>::iterator (iter) = (l)->begin(); \
            (iter) != (l)->end(); \
            ++(iter))
typedef struct _edgeS {
    int u;
    int v;
} edgeT;

list<edgeT*> *adjEdge;
list<edgeT*> all_edges;
#define LIST_ITERATE_EDGE( iter, l ) \
    for (std::list<edgeT*>::iterator (iter) = (l)->begin(); \
            (iter) != (l)->end(); \
            ++(iter))

int other(edgeT* e, int i)
{
    int j = -1;
    if (e) {
        if( e->u == i ) return e->v;
        if( e->v == i ) return e->u;
    }
    return -1;
}

edgeT* find_edge( list<edgeT*> edges, int i ) 
{
    edgeT* e = NULL;
    LIST_ITERATE_EDGE( li, &edges ) {
        e = *li;
        //if ((e->u == i) || (e->v == i)) {
        if (other(e, i) > 0) {
            return e;
        }
    }
    return NULL;
}


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
                int v = (int)val;
                edgeT* e = new edgeT;
                if (!(e = find_edge(adjEdge[v], node))) {
                    e = new edgeT;
                    all_edges.push_back(e);
                    e->u = node;
                    e->v = v;
                }
                adj[node].push_back(v);
                adjEdge[node].push_back(e);
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
    for (int i = 0; i <= sz; ++i) {
        if (!adj[i].empty()) {
            printf("\n%d =>", i);
            LIST_ITERATE_EDGE(vi, &adjEdge[i]) {
                edgeT* e = *vi;
                printf(" %d", other(e, i));
            }
        }
    }
    printf("\n");
    edgeT* e = NULL;
    printf("Edges:\n");
    int cnt = 1;
    LIST_ITERATE_EDGE(et, &all_edges) {
        e = *et;
        printf("%d. (%d, %d)\n", cnt++, e->u, e->v);
    }
}

int main(int argc, char* argv[])
{
    char *file  = argv[1];

    sz      = atoi(argv[2]);
    adj     = new list<int>[sz + 1];
    adjEdge = new list<edgeT*>[sz + 1];
    printf("File: %s, arr size: %d\n", file, sz);
    parse(file);
    print_graph();

    delete [] adj;
    delete [] adjEdge;

    return 0;
}
