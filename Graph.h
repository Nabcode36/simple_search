// Interface to the directed Weighted Graph ADT

// The following code has been heavily inspired from lab08, with the removal of functions 
// related to minimum spanning trees and making the graph directed instead of undirected. It can be 
// found in z5359531's submission page: https://cgi.cse.unsw.edu.au/~cs2521/22T3/view/main.cgi/wed11-kora/5359531/

// - Vertices are identified by integers between 0 and nV - 1,
//   where nV is the number of vertices in the graph
// - Weights are doubles and must be positive
// - Self-loops are not allowed

#ifndef GRAPH_H
#define GRAPH_H

#include <stdbool.h>

typedef struct graph {
    int nV;         // number of vertices
    int nE;         // number of edges
    double **edges; // adjacency matrix storing positive weights
                    // 0 if nodes not adjacent
} *Graph;

typedef int Vertex;

// edges are pairs of vertices (end-points)
typedef struct Edge {
    Vertex v;
    Vertex w;
    double weight;
} Edge;

// Creates a new instance of a graph
Graph  GraphNew(int nV);

// Frees all memory associated with the given graph
void   GraphFree(Graph g);

// Returns the number of vertices in the graph
int    GraphNumVertices(Graph g);

// Inserts  an  edge into a graph. Does nothing if there is already an
// edge between `e.v` and `e.w`. Returns true if successful, and false
// if there was already an edge.
bool   GraphInsertEdge(Graph g, Edge e);

// Removes an edge from a graph. Returns true if successful, and false
// if the edge did not exist.
bool   GraphRemoveEdge(Graph g, Vertex v, Vertex w);

// Returns the weight of the edge between `v` and `w` if it exists, or 
double GraphIsAdjacent(Graph g, Vertex v, Vertex w);

// Displays information about the graph
void   GraphShow(Graph g);

#endif
