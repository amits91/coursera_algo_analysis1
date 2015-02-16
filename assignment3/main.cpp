#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <list>
#include <vector>
#include <time.h>
using namespace std;


int sz = 0;
vector<list<int> > adj;
#define LIST_ITERATE( iter, l ) \
    for (std::list<int>::iterator (iter) = (l)->begin(); \
            (iter) != (l)->end(); \
            ++(iter))
typedef struct _edgeS {
    int u;
    int v;
} edgeT;

vector<list<edgeT*> > adjEdge;
#define LIST_ITERATE_EDGE( iter, l ) \
    for (std::list<edgeT*>::iterator (iter) = (l)->begin(); \
            (iter) != (l)->end(); \
            ++(iter))
vector<edgeT*> all_edges;
#define VECTOR_ITERATE_EDGE( iter, l ) \
    for (std::vector<edgeT*>::iterator (iter) = (l)->begin(); \
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
            //printf(" %ld", val);
        }
        //printf("\nL:\t");
        //printf("%s", line);
    }
    free(line);

}

char* print_edge( edgeT* e )
{
    static char s[1024];
    sprintf( s, "(%d, %d)", e->u, e->v );
    return s;
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
    VECTOR_ITERATE_EDGE(et, &all_edges) {
        e = *et;
        printf("%d. (%d, %d)\n", cnt++, e->u, e->v);
    }
}

void print_list( list <edgeT*> l, int i )
{
    printf("%d =>", i);
    LIST_ITERATE_EDGE(vi, &l) {
        edgeT* e = *vi;
        printf(" %s", print_edge(e));
    }
    printf("\n");
}

void print_g( vector<list <edgeT*> > g)
{
    for (int i = 0; i <= sz; ++i) {
        if (!g[i].empty()) {
            print_list(g[i], i);
        }
    }
    printf("\n");
}

int choose_random_num( int n )
{
    return (rand() % n);
}

edgeT* get_random_edge( vector<edgeT*> &edges )
{
    int ri = choose_random_num( edges.size() );
    edgeT* er = edges[ri];
    edges[ri] = edges.back();
    edges.pop_back();
    return er;
}



/*********************************************************
 * Random Contraction Algorithm                          *
 * While there are more than 2 vertices:                 *
 * • pick a remaining edge (u,v) uniformly at random     *
 * • merge (or “contract” ) u and v into a single vertex *
 * • remove self-loops                                   *
 * return cut represented by final 2 vertices.           *
 *********************************************************/

int merge( edgeT* e, vector<list <edgeT*> > &adj, int * &id )
{
    int u = id[e->u];
    int v = id[e->v];
    int t = u;

    if (u > v) {
        t = u;
        u = v;
        v = t;
    }

    printf("Merge: %s\n", print_edge(e) );
    printf("Before\n");
    print_list(adj[u], u);
    print_list(adj[v], v);

    adj[u].remove(e);
    adj[v].remove(e);
    printf("Remove\n");
    print_list(adj[u], u);
    print_list(adj[v], v);
    while( adj[v].size() > 0 ) {
        adj[u].push_back(adj[v].back());
        adj[v].pop_back();
    }
    printf("After\n");
    print_list(adj[u], u);
    printf("Change %d to %d\n", v, u);
    id[e->v] = u;

    return 0;
}

int rca( )
{
    vector<edgeT*> edges (all_edges);
    vector<list <edgeT*> > adj (adjEdge);
    int cnt = 1;
    int n = sz;
    int *id = (int*)malloc( (sz + 1) * sizeof(int));
    printf( "Graph\n" );
    print_g(adj);
    for (int i = 0; i <= sz; ++i) {
        id[i] = i;
    }
    while( n > 2 ) {
        edgeT* e = get_random_edge(edges);
        n = merge(e, adj, id);
    }
    printf( "Mincut Graph\n" );
    print_g(adj);
    free(id);
}

int main(int argc, char* argv[])
{
    char *file  = argv[1];

    sz      = atoi(argv[2]);
    adj.resize(sz + 1);
    adjEdge.resize(sz + 1);
    printf("File: %s, arr size: %d\n", file, sz);
    srand ( time(NULL) ); //initialize the random seed
    parse(file);
    //print_graph();
//    for (int i = 0; i <= sz; ++i) {
        rca();
//    }

    //delete [] adj;
    //delete [] adjEdge;

    return 0;
}
