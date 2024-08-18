#ifndef READDATA_H
#define READDATA_H

#define MAX_URL_LEN 100

#include "Graph.h"

// Given the collection file containing all the urls and the number of urls
// in the file, make a graph with the urls as vertices. the numbered vertices
// will be stored in the mapNumtourl array which will link vertice (index) numbers 
// to the url names
Graph pageGraph(FILE *collection, char mapNumtoUrl[][MAX_URL_LEN + 4], int noUrls);

// Given the new graph, creates edges between the vertices based on the references
// in each url text file
void links(Graph urls, char numToUrl[][MAX_URL_LEN + 4]);

#endif