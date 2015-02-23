#include "sys/time.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <list>
#include <vector>
#include <queue>
#include <time.h>
#include <algorithm>
using namespace std;

#define DEBUG

#define INIT_TIME_SEG initTimeSeg()
#define RECORD_TIME_SEGMENT(x) recordTimeSeg((x))
double wtime(void)
{
    double sec;
    struct timeval tv;
    gettimeofday(&tv,NULL);
    sec = tv.tv_sec + tv.tv_usec/1000000.0;
    return sec;
}

double initTimeSeg()
{
    static double startTime = 0; 
    double lastStartTime = startTime;
    startTime = wtime();
    return lastStartTime;
}

void recordTimeSeg(const char* msg)
{
    static int cnt = 0;
    printf("#### %10d: Time(%20s): %f\n", cnt++, (msg?msg:""), wtime()-initTimeSeg());
    fflush(stdout);
}

typedef struct _edgeS edgeT;
typedef struct _vertexS vertexT;
typedef vector<vertexT*> graphT;

struct _edgeS {
    vertexT* tail;
    vertexT* head;
    int length;
};
struct _vertexS {
    int id;
    list<edgeT*> adjEdges;
};
#define VECTOR_ITERATE( iter, l ) \
    for (std::vector<vertexT*>::iterator (iter) = (l)->begin(); \
            (iter) != (l)->end(); \
            ++(iter))
#define EDGE_ITERATE( iter, l ) \
    for (std::list<edgeT*>::iterator (iter) = (l)->begin(); \
            (iter) != (l)->end(); \
            ++(iter))

vertexT* get_or_create_vertex( graphT &g, int id)
{
    vertexT* v = g[id];

    if (!v) {
        v = new vertexT;
        v->id = id;
        g[id] = v;
    }
    return v;
}

edgeT* find_edge(vertexT* v, int id)
{
    edgeT* e = NULL;

    EDGE_ITERATE(et, &v->adjEdges) {
        edgeT* ed = *et;
        if (ed->head->id == id) {
            e = ed;
        }
    }
    return e;
}

static void parse(char* file, graphT &g)
{   
    FILE* f = fopen(file, "r");
    ssize_t read;
    char *line = NULL;
    size_t len = 0;

    if (!f) {
        printf("Error: File: %s not opened\n", file);
        exit(EXIT_FAILURE);
    }
    while ((read = getline(&line, &len, f)) != -1) {
        char *endptr, *str, *prev_endptr;
        long val;
        bool first = true;
        vertexT* node = NULL;
        edgeT* edge = NULL;
        
        str = line;
        prev_endptr = line;
        while( 1 ) {
            errno = 0;    /* To distinguish success/failure after call */
            val = strtol(str, &endptr, 10);
            if( errno ) exit(EXIT_FAILURE);
            if( !str ) break;
            if( str == endptr ) break;

            str = endptr;
            if( first ) {
                first = false;
                node = get_or_create_vertex(g, (int)val);
            } else if (*endptr == ',') {
                vertexT* head = get_or_create_vertex(g, val);
                edge = find_edge(node, val);
                if (!edge) {
                    edge = new edgeT;
                    edge->tail = node;
                    edge->head = head;
                    edge->length = 0;
                    node->adjEdges.push_back(edge);
                }
                ++str;
            } else if (*prev_endptr == ',') {
                edge->length = val;
            }
            prev_endptr = endptr;
        }
    }
    free(line);
    fclose(f);
}

void print_node( vertexT* n )
{
    printf("%d", n->id);
    printf(" =>", n->id);
    EDGE_ITERATE(vi, &n->adjEdges) {
        edgeT* e = *vi;
        printf(" %d,%d", e->head->id, e->length);
    }
    printf("\n");
}

void print_gi( graphT &g)
{
#ifdef DEBUG
    printf("Graph: \n");
    for (int i = 0; i < g.size(); ++i) {
        if (g[i]) {
            print_node(g[i]);
        }
    }
    printf("\n");
#endif
}

void dump_dot(graphT &g)
{
    FILE* f = fopen("g.dot", "w");
    fprintf( f, "digraph g {\n");
    VECTOR_ITERATE(vi, &g) {
        vertexT* v = *vi;
        if (v) {
            EDGE_ITERATE(li, &v->adjEdges) {
                edgeT* e = *li;
                fprintf(f, "%d -> %d;\n", v->id, e->head->id); 
            }
        }
    }
    fprintf(f, "}\n");
    fclose(f);
}

void dijkstra(graphT &g, int s, vector<int> &A, vector<graphT> &B)
{
    vector<vertexT*> X;
    vector<char> m(g.size(), false);
    vertexT* start = NULL;

    start = get_or_create_vertex(g, s);
    A[start->id] = 0;
    B[start->id].clear();
    m[start->id] = true;
    X.push_back(start);

    while (X.size() < (g.size() - 1)) {
        int minA = 1000000;
        edgeT* minE = NULL;

        // Find minA from edges crossing the
        // frontier
        VECTOR_ITERATE(xi, &X) {
            vertexT* v = *xi;
            EDGE_ITERATE(ei, &v->adjEdges) {
                edgeT* e = *ei;
                if (!m[e->head->id]) {
                    int wA = A[e->tail->id] + e->length;
                    if (wA < minA) {
                        minA = wA;
                        minE = e;
                    }
                }
            }
        }
        if (minE) {
            X.push_back(minE->head);
            m[minE->head->id] = true;
            A[minE->head->id] = minA;
            B[minE->head->id] = B[minE->tail->id];
            B[minE->head->id].push_back(minE->head);
        }
    }
}

int main(int argc, char* argv[])
{
    char *file = argv[1];
    int   sz   = atoi(argv[2]);
    graphT oG;

    INIT_TIME_SEG;
    oG.resize(sz + 1);

    parse(file, oG);
    RECORD_TIME_SEGMENT("Parsing");
    print_gi(oG);

#ifdef DEBUG
    dump_dot(oG);
    RECORD_TIME_SEGMENT("Dumping Dotty file");
#endif
    vector<int> A(oG.size(), 1000000);
    vector<graphT> B(oG.size());
    dijkstra(oG, 1, A, B);
#ifdef DEBUG
    for (int i = 1; i < A.size(); ++i) {
        printf("%d %d [", oG[i]->id, A[i]);
        if (B[i].size() > 0) {
            graphT &sp = B[i];
            for (int j = 0; j < sp.size(); ++j) {
                printf("%d, ", sp[j]->id);
            }
        }
        printf("]\n");
    }
#endif
    if (sz == 200) {
        vector<int> ip;
        ip.push_back(7);
        ip.push_back(37);
        ip.push_back(59);
        ip.push_back(82);
        ip.push_back(99);
        ip.push_back(115);
        ip.push_back(133);
        ip.push_back(165);
        ip.push_back(188);
        ip.push_back(197);
        printf("\n");
        for (int i = 0; i < ip.size(); ++i) {
            printf("%4d,", ip[i]);
        }
        printf("\n");
        for (int i = 0; i < ip.size(); ++i) {
            printf("%4d,", A[ip[i]]);
        }
        printf("\n");
    }
    return 0;
}
