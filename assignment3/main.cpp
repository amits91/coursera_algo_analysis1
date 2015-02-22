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
    fclose(f);

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

int choose_random_num( int n )
{
    return (rand() % n);
}

void get_random_edge( graphT &g, int &u, int &v )
{
    int ri = choose_random_num( g.size() );
    vertexT er = g[ri];
    if( er.size() > 0 ) {
        u = er[0];
        int ei = choose_random_num( (er.size() - 1) ) + 1;
        v = er[ei];
    }
}


int lookup( map<int, int> *tab, int i)
{
    return tab->find(i)->second;
}


/*********************************************************
 * Random Contraction Algorithm                          *
 * While there are more than 2 vertices:                 *
 * • pick a remaining edge (u,v) uniformly at random     *
 * • merge (or “contract” ) u and v into a single vertex *
 * • remove self-loops                                   *
 * return cut represented by final 2 vertices.           *
 *********************************************************/

int find_edge( vector<int> &v, int j )
{
    for (int i = 1; i < v.size(); ++i) {
        if( v[i] == j ) return j;
    }
    return 0;
}

void merge(graphT &g, map<int, int> *tab, int u, int v)
{
    int t = u;

    if (u > v) {
        t = u;
        u = v;
        v = t;
    }
    int ui = lookup(tab,u);
    int vi = lookup(tab,v);
//    printf("(%d, %d)\n", u, v);
//    print_vector(g[ui]);
//    print_vector(g[vi]);
    for (int i = 1; i < g[ui].size(); ++i) {
        if (g[ui][i] == v) {
            g[ui][i] = u;
        }
    }

//    printf("changing conns: { \n");
    for (int i = 1; i < g[vi].size(); ++i) {
        g[ui].push_back(g[vi][i]);
//        if (g[vi][i] != u) {
//            if (!find_edge(g[ui], g[vi][i])) {
//            }
            int vvi = lookup(tab,g[vi][i]);
            bool refo = false;
//            printf("cons of %d => %d => %d \n", v, g[vi][i], vvi);
//            print_vector(g[vvi]);
            refo = find_edge(g[vvi], u);
            // Change references to v from all its connections
            for (int j = 1; j < g[vvi].size(); ++j) {
                if (g[vvi][j] == v) {
//                    if (refo) {
//                        g[vvi][j] = g[vvi].back();
//                        g[vvi].pop_back();
//                    } else {
                        g[vvi][j] = u;
//                    }
                }
            }
//            print_vector(g[vvi]);
//        }
    }
//    printf(" } \n");
//    printf("Newv %d:\n", u );
//    print_vector(g[ui]);
//    printf("after removing self loops:\n");
#if 1
    vertexT nv;
    nv.push_back(u);
    for (int i = 1; i < g[ui].size(); ++i) {
        if (g[ui][i] != u) {
            nv.push_back(g[ui][i]);
        }
    }
    g[ui] = nv;
#endif
//    print_vector(g[ui]);
    g[vi] = g.back();
//    printf("changing id of %d to %d\n", g[vi].front(), vi );
    (*tab)[g[vi].front()] = vi;
    g.pop_back();
}

int rca(graphT &g )
{
    graphT lG;;
    int cnt = 1;
    int n = sz;
    map<int, int> tab;
    lG.resize(sz);
    for (int i = 1; i <= sz; ++i) {
        lG[i-1] = g[i];
        tab[i] = i - 1;
    }
//    printf( "Graph\n" );
//    print_gi(lG);
    while( lG.size() > 2 ) {
//    while( m > 2 ) {
        int u = -1;
        int v = -1;
        get_random_edge(lG, u, v);
//        printf("(%d, %d)\n", u, v);
        merge(lG, &tab, u, v);
//        --m;
    }
//    printf( "Mincut Graph\n" );
//    print_gi(lG);
    return max(lG[0].size(), lG[1].size()) - 1;
}

int main(int argc, char* argv[])
{
    char *file  = argv[1];
    int minc = 100000000;

    sz      = atoi(argv[2]);
    oG.resize(sz + 1);
    printf("File: %s, arr size: %d\n", file, sz);
    srand ( time(NULL) ); //initialize the random seed
    parse(file);
    for( int i = 0; i < sz*sz; ++i) {
        int mi = rca(oG);
        printf(" mi : %d \t %d\n", mi, minc);
        if( mi < minc ) minc = mi;
    }
    //printf("Original Graph: \n");
    //print_gi(oG);
    printf(" minc : %d\n", minc);

    return 0;
}
