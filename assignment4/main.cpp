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

int main(int argc, char* argv[])
{
    char *file  = argv[1];

    sz      = atoi(argv[2]);
    oG.resize(sz + 1);
    parse(file);
    print_gi(oG);
    return 0;
}
