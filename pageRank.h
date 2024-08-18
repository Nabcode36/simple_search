#ifndef PAGERANK_H
#define PAGERANK_H

#include "readData.h"

// Stores wIn and wOut
typedef struct weights {
    double wIn;
    double wOut;
} Weights;

// Stores information about each url's name, outdegree and pagerank
typedef struct urlInfo {
    char name[MAX_URL_LEN + 4];
    double outDeg;
    double rank;
} urlData;

// Ranks each URL in descending order based on its inlinks and outlinks
void pageRank(double d, double diffPR, int maxIterations);

// Calculates the number of inlinks and outlinks for every URL/node in the graph
void linksCompute(Graph urlLinks, int noUrls, double inLinks[], double outLinks[]);

// Calculates wIn and wOut for each edge
void weightsCompute(Graph urlLinks, int noUrls, double inLinks[], double outLinks[], Weights w[][noUrls]);

// Sorts the pageRanks array in increasing order based on their rank, and if urls share the same
// rank they are sorted in descending order based on their alphabeetical value as defined by strcmp
// The sorted array will be printed in reverse order to match the output format in the spec
void sortRanks(urlData uD[], int lo, int hi);

#endif