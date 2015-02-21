#include "sys/time.h"
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
int sz = 0;

typedef struct _vertexS vertexT;
struct _vertexS {
    int id;
    list<vertexT*> adjEdges;
    list<vertexT*> adjRevEdges;
};
typedef vector<vertexT*> graphT;
graphT oG;
#define VECTOR_ITERATE( iter, l ) \
    for (std::vector<vertexT*>::iterator (iter) = (l)->begin(); \
            (iter) != (l)->end(); \
            ++(iter))
#define VECTOR_ITERATE_REV( iter, l ) \
    for (std::vector<vertexT*>::iterator (iter) = (l)->end(); \
            (iter) != (l)->begin(); \
            --(iter))
#define LIST_ITERATE( iter, l ) \
    for (std::list<vertexT*>::iterator (iter) = (l)->begin(); \
            (iter) != (l)->end(); \
            ++(iter))

static void parse(char* file)
{   
    ssize_t read;
    FILE* f = fopen(file, "r");
    int v = 0;
    int e = 0;
    if (!f) {
        printf("Error: File: %s not opened\n", file);
        exit(EXIT_FAILURE);
    }
    while (fscanf(f, "%d %d", &v, &e) == 2) {
        vertexT* node = oG[v];
        vertexT* connNode = oG[e];
        if (!node) {
            node = new vertexT;
            node->id = v;
            oG[v] = node;
        }
        if (!connNode) {
            connNode = new vertexT;
            connNode->id = e;
            oG[e] = connNode;
        }
        node->adjEdges.push_back(connNode);
        connNode->adjRevEdges.push_back(node);
    }
    fclose(f);
}

void print_node( vertexT* n, bool onlyId )
{
    printf("%d", n->id);
    if (onlyId == false) {
        printf(" =>", n->id);
        LIST_ITERATE(vi, &n->adjEdges) {
            vertexT* e = *vi;
            printf(" %d", e->id);
        }
        printf("\n");
        printf("\t%d [R]=> ", n->id);
        LIST_ITERATE(vi, &n->adjRevEdges) {
            vertexT* e = *vi;
            printf(" %d", e->id);
        }
    }
    printf("\n");
}

void print_gi( graphT &g, const char* s, bool onlyId)
{
    printf("Graph: %s\n", s);
#ifdef DEBUG
    for (int i = 0; i < g.size(); ++i) {
        if (g[i]) {
            print_node(g[i], onlyId);
        }
    }
    printf("\n");
#endif
}

void dfs( graphT &g,
        vertexT* v,
        vector<char> &marker,
        int &t,
        graphT &ng,
        bool rev,
        vector<int> &finish
        )
{
    marker[v->id] = true;
    list<vertexT*> *edges;
    edges = (rev? &v->adjRevEdges : &v->adjEdges);
    if (rev == false) {
        ng.push_back(v);
    }
    LIST_ITERATE(ei, edges) {
        vertexT* e = *ei;
        if (!marker[e->id]) {
            dfs(g, e, marker, t, ng, rev, finish);
        }
    }
    ++t;
    if (rev == true) {
        // ng[ng.size() - t] = v;
        ng[t] = v;
        finish[v->id] = t;
    }
}

void dfsLoop( graphT &g, graphT &ng, bool rev, vector<char> &marker )
{
    int t = 0;
    vector<int> finish(g.size());
    marker.assign(g.size(), false);
    VECTOR_ITERATE_REV(ni, &g) {
        vertexT* v = *ni;
        if (!v) {
            continue;
        }
        if (!marker[v->id]) {
            dfs(g, v, marker, t, ng, rev, finish);
        }
    }
#ifdef DEBUG
    if (rev == true) {
        printf("Finish Times: \n");
        for (int i = 1; i < g.size(); ++i) {
            printf("%d : %d\n", i, finish[i]);
        }
    }
#endif
}
void dump_dot(graphT &g)
{
    FILE* f = fopen("g.dot", "w");
    fprintf( f, "digraph g {\n");
    VECTOR_ITERATE(vi, &g) {
        vertexT* v = *vi;
        if (v) {
            LIST_ITERATE(li, &v->adjEdges) {
                vertexT* e = *li;
                fprintf(f, "%d -> %d;\n", v->id, e->id); 
            }
        }
    }
    fprintf(f, "}\n");
    fclose(f);
    f = fopen("gRev.dot", "w");
    fprintf(f, "digraph gRev {\n");
    VECTOR_ITERATE(vi, &g) {
        vertexT* v = *vi;
        if (v) {
            LIST_ITERATE(li, &v->adjRevEdges) {
                vertexT* e = *li;
                fprintf(f, "%d -> %d;\n", v->id, e->id); 
            }
        }
    }
    fprintf(f, "}\n");
    fclose(f);
}

int main(int argc, char* argv[])
{
    char *file  = argv[1];
    graphT ng;

    INIT_TIME_SEG;
    sz      = atoi(argv[2]);
    oG.resize(sz + 1);
    ng.resize(sz + 1);

    parse(file);
    RECORD_TIME_SEGMENT("Parsing");
    print_gi(oG, "Original Graph", false);

    vector<char> marker(ng.size(), false);
    vector<vertexT*> leaders;
    dump_dot(oG);

    dfsLoop(oG, ng, true, marker);
    RECORD_TIME_SEGMENT("DFS-loop Reverse");
    print_gi(ng, "Finish Time Graph", false);

    dfsLoop(ng, leaders, false, marker);
    RECORD_TIME_SEGMENT("DFS-loop ");
    for (int i = 0; i < leaders.size(); ++i) {
        printf("%d\n", leaders[i]->id);
    }
    return 0;
}
