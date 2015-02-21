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
    for (int i = 0; i < g.size(); ++i) {
        if (g[i]) {
            print_node(g[i], onlyId);
        }
    }
    printf("\n");
}

void dfs( graphT &g,  vertexT* v, vector<char> &marker, int &t, graphT &ng, bool rev )
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
            dfs(g, e, marker, t, ng, rev);
        }
    }
    ++t;
    if (rev == true) {
        ng[ng.size() - t] = v;
    }
}

void dfsLoop( graphT &g, graphT &ng, bool rev, vector<char> &marker )
{
    int t = 0;
    marker.assign(g.size(), false);
    VECTOR_ITERATE(ni, &g) {
        vertexT* v = *ni;
        if (!v) {
            continue;
        }
        if (!marker[v->id]) {
            dfs(g, v, marker, t, ng, rev);
        }
    }
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
    //print_gi(oG, "Original Graph", false);

    vector<char> marker(ng.size(), false);
    vector<vertexT*> leaders;

    dfsLoop(oG, ng, true, marker);
    RECORD_TIME_SEGMENT("DFS-loop Reverse");
    //print_gi(ng, "Finish Time Graph", false);

    dfsLoop(ng, leaders, false, marker);
    RECORD_TIME_SEGMENT("DFS-loop ");
    print_gi(leaders, "Leaders",  true);
    return 0;
}
