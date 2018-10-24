// Implementation of adjacency list graph ADT
// Completed with code from COMP2521 week05b lectures
// With reference to week09 lab files "Graph.c" & "Graph.h"
// Written by Steven Deng and Ying Zhong, October 2018

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "graph.h"
#include "graphList.h"
#include <math.h>

typedef struct GraphRep {
   List *edges;  // array of lists
   int    nV;     // #vertices
   int    nE;     // #edges
} GraphRep;

Graph newGraph (int nV) {
    assert(nV >= 0);
    int i;

    Graph g = malloc(sizeof(GraphRep));
    assert (g != NULL);
    g->nV = nV;
    g->nE = 0;
    g->edges = malloc(nV * sizeof(List));
    assert(g->edges != NULL);
    for (i = 0; i < g->nV; i++) {	//newNode needs to go here
        g->edges[i] = NULL;			//g->edges[i] != NULL
    }

    return g;
}

// check validity of Vertex
bool validV(Graph g, Vertex v)
{
	return (g != NULL && v >= 0 && v < g->nV);
}

void insertEdge (Graph g, Edge e)   {
    //from = e.v; to = e.w
    assert(g != NULL && validV(g, e.v) && validV(g, e.w));
    if (!inLL(g->edges[e.v], e.w))  {   //edge e not in graph
        g->edges[e.v] = insertLL(g->edges[e.v], e.w);
        g->nE++;
    }
}

void removeEdge (Graph g, Edge e)   {
    //from = e.v; to = e.w
    assert (g != NULL && validV(g, e.v) && validV(g, e.w));
    if (inLL(g->edges[e.v], e.w))   {   //edge e in graph
        g->edges[e.v] = deleteLL(g->edges[e.v], e.w);
        g->nE--;
    }
}

bool adjacent (Graph g, Vertex v, Vertex w)   {
    assert(g != NULL && validV(g,v));
    return inLL(g->edges[v], w);
}

void showGraph (Graph g)    {
    assert(g != NULL);
    int i;
    for (i = 0; i < g->nV; i++) {
        printf("%d - ", i);
        showLL(g->edges[i]);
    }
    printf("Number of vertices: %d\n", g->nV);
    printf("Number of edges: %d\n", g->nE);
}

void freeGraph (Graph g)    {
    assert(g != NULL);
    int i;

    for (i = 0; i < g->nV; i++) {
        freeLL(g->edges[i]);
    }
    free(g);
}

Connections nInOutLinks (Graph g, Vertex v) {
    assert(g->edges[v] != NULL);
    Connections c;
    c.out = nOutLL(g->edges[v]);
    c.in = 0;
    int i = 0;
    while (i < g->nV && g->edges[i] != NULL)    {
        if (i != v) {
            c.in = c.in + nInLL(g->edges[i], v);
        }   
        i++;
    }
    return c;
}

float Iu (Graph g, Vertex v)    {
    float sum = 0;
    int i = 0;
    while (i < g->nV && g->edges[i] != NULL)    {
        if (i != v) {
            sum = sum + nInLL(g->edges[i], v);
        }
        i++;
    }
    return sum;
}

float sumIp (Graph g, Vertex v)   {
    char *array = outArray(g->edges[v]);
    int i;
    float sum = 0;
    for (i = 0; i < nOutLL(g->edges[v]); i++)  {
        sum = sum + Iu(g, array[i]);
    }
    return sum;
}

float Win (Graph g, Edge e) {
    float I_u = Iu(g, e.w);
    float sum_Ip = sumIp(g, e.v);
    return I_u/sum_Ip;
}

float Ou (Graph g, Vertex v)    {
    assert(g->edges[v] != NULL);
    float out = nOutLL(g->edges[v]);
    if (out == 0)   {
        return 0.5;
    }
    return out;
}

float sumOp (Graph g, Vertex v)   {
    char *array = outArray(g->edges[v]);
    int i;
    float sum = 0;
    for (i = 0; i < nOutLL(g->edges[v]); i++)  {
        sum = sum + Ou(g, array[i]);
    }
    return sum;
}

float Wout (Graph g, Edge e) {
    float O_u = Ou(g, e.w);
    float sum_Op = sumOp(g, e.v);
    return O_u/sum_Op;
}

// return array of incoming urls to a page
char *inArray(Graph g, Vertex v)   {
    char *in = malloc(nInLL(g->edges[v], v));
    assert(in != NULL);

    int i = 0;
    int j = 0;
    while (i < g->nV && g->edges[i] != NULL)    {
        if (i != v && inLL(g->edges[i], v)) {
            in[j] = i;
            j++;
        }   
        i++;
    }
    return in;
}

// print array of incoming urls to a page
void printArray (Graph g, Vertex v) {
    char *ar = inArray(g, v);
    int i;
    for (i = 0; i < Iu(g, v); i++)    {
        printf("%d\n", ar[i]);
    }
}

// return PR*Win*Wout for a given edge
float PRWinWout (float PR, Graph g, Edge e) {
    return PR*Win(g, e)*Wout(g,e);
}

// return sum of PRWinWout for a page
float sumPRWinWout (float PR, Graph g, Vertex v)   {
    char *array = inArray(g, v);
    float sum = 0;
    int i;
    Edge e;
    for (i = 0; i < Iu(g, v); i++)    {
        e.v = array[i];
        e.w = v;
        sum = sum + PRWinWout(PR, g, e);
    }
    return sum;
}

// return PageRank for a page
float PageRank (Graph g, Vertex v, float PR, float d)    {
    int N = g->nV;
    float weightedPR;
    weightedPR = ((1-d)/N) + (d * sumPRWinWout(PR, g, v));
    return weightedPR;
}

    