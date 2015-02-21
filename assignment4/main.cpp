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
    vector<vertexT*> adjEdges;
    vector<vertexT*> adjRevEdges;
};
typedef vector<vertexT*> graphT;
graphT oG;
#define VECTOR_ITERATE( iter, l ) \
    for (std::vector<vertexT*>::iterator (iter) = (l)->begin(); \
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

void print_node( vertexT* n )
{
    printf("%d => ", n->id);
    VECTOR_ITERATE(vi, &n->adjEdges) {
        vertexT* e = *vi;
        printf(" %d", e->id);
    }
    printf("\n");
    printf("%d [R]=> ", n->id);
    VECTOR_ITERATE(vi, &n->adjRevEdges) {
        vertexT* e = *vi;
        printf(" %d", e->id);
    }
    printf("\n");
}

void print_gi( graphT &g)
{
    for (int i = 0; i < g.size(); ++i) {
        if (g[i]) {
            print_node(g[i]);
        }
    }
    printf("\n");
}

void dfsLoop( graphT &g, bool rev )
{
    vector<bool> marker;
    marker.assign(g.size(), false);
    VECTOR_ITERATE(ni, &g) {
        vertexT* v = *ni;
        if (!v) {
            continue;
        }
        if (!marker[v->id]) {
            //dfs(v, marker, rev);
        }
    }
}

int main(int argc, char* argv[])
{
    char *file  = argv[1];

    INIT_TIME_SEG;
    sz      = atoi(argv[2]);
    oG.resize(sz + 1);
    parse(file);
    RECORD_TIME_SEGMENT("Parsing");
    dfsLoop(oG, true);
    RECORD_TIME_SEGMENT("DFS-loop Reverse");
    dfsLoop(oG, false);
    RECORD_TIME_SEGMENT("DFS-loop ");
    print_gi(oG);
    RECORD_TIME_SEGMENT("Printing Graph");
    return 0;
}
