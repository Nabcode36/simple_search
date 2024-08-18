// COMP2521 Assignment 2

// Written by: Nabeel Pathan (z5359531)
// Date: 8/11/2022

#include <assert.h>
#include <ctype.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pageRank.h"

int main(int argc, char *argv[]) {
    // argc is the number of command-line arguments, which includes the
    // program name
    // argv is the array of command-line arguments
    if (argc != 4) {
        fprintf(stderr, "Usage: %s dampingFactor diffPR maxIterations\n",
                argv[0]);
        return EXIT_FAILURE;
    }

    pageRank(atof(argv[1]), atof(argv[2]), atoi(argv[3]));
    // Command-line arguments are received as strings. They can be converted
    // to number types using atoi and atof.
    // double d = atof(argv[1]);
    // double diffPR = atof(argv[2]);
    // int maxIterations = atoi(argv[3]);
}

void pageRank(double d, double diffPR, int maxIterations) {
    FILE *fp = fopen("collection.txt", "r");
    if (fp == NULL) {
        fprintf(stderr, "Can't open collection.txt\n");
        exit(EXIT_FAILURE);
    }
    
    char word[MAX_URL_LEN];
    int noUrls = 0; // This will be N in the pageRank psuedocode
    while ((fscanf(fp, "%s", word)) != EOF) {
        noUrls++;
    }
    
    char mapNumtoUrl[noUrls][MAX_URL_LEN + 4];
    Graph urlLinks = pageGraph(fp, mapNumtoUrl, noUrls);
    
    // Initialising the Weights array. Stores wIn and wOut for each edge
    Weights w[noUrls][noUrls];
    for (int i = 0; i < noUrls; i++) {
        for (int j = 0; j < noUrls; j++) {
            Weights link;
            link.wIn = 0.0;
            link.wOut = 0.0;
            w[i][j] = link;
        }
    } 

    // Initialising the inLinks array. Stores the amount of urls that has
    // the url as part of its references
    double inLinks[noUrls];
    for (int i = 0; i < noUrls; i++) {
        inLinks[i] = 0.0;
    }

    // Initialising the outLinks array. Stores the amount of urls that it 
    // references. It is set to 0.5 by deafult due to the assignment 2 part 1 
    // spec. Avoids division by 0
    double outLinks[noUrls];
    for (int i = 0; i < noUrls; i++) {
        outLinks[i] = 0.5;
    }

    linksCompute(urlLinks, noUrls, inLinks, outLinks);
    weightsCompute(urlLinks, noUrls, inLinks, outLinks, w);

    // Initialising the initRanks array. This will be the ranks of urls in 
    // iteration t
    double initRanks[noUrls];
    for (int i = 0; i < noUrls; i++) {
        initRanks[i] = 1.0/noUrls;
    }

    // Initialising the initRanks array. This will be the ranks of urls in 
    // iteration t + 1
    double newRanks[noUrls];
    for (int i = 0; i < noUrls; i++) {
        newRanks[i] = 1.0/noUrls;
    }

    int iteration = 0;
    double diff = diffPR;
    
    // This loop is heavily based on the pageRank formula provided in
    // the assignment 2 part 1 spec: https://cgi.cse.unsw.edu.au/~cs2521/22T3/assignments/ass2
    while (iteration < maxIterations && diff >= diffPR) {
        for (int i = 0; i < noUrls; i++) {
            // Calculating sigma from the pageRank formula 
            // for all outgoing links from p(i)
            double sigma = 0.0;
            for (int j = 0; j < noUrls; j++) {
                if (GraphIsAdjacent(urlLinks, j, i) != 0.0) {
                    double sigmaStage = initRanks[j] * w[j][i].wIn * w[j][i].wOut;
                    sigma += sigmaStage;
                }
            }
            newRanks[i] = ((1.0 - d)/noUrls) + (d * sigma);
        }
        
        // Updating diff after calculating the new pageranks
        double sigmaDiff = 0.0;
        for (int i = 0; i < noUrls; i++) {
            sigmaDiff += fabs(newRanks[i] - initRanks[i]);
        }   

        diff = sigmaDiff;

        // newRanks becomes the initRanks in the next iteration
        for (int i = 0; i < noUrls; i++) {
            initRanks[i] = newRanks[i];
        }

        iteration++; 
    }

    // Making an array of structs containing information about urls.
    // More information about the structs is in pageRank.h. This was done
    // to make sorting the pages easier.
    urlData uD[noUrls];
    for (int i = 0; i < noUrls; i++) {
        strcpy(uD[i].name, mapNumtoUrl[i]);
        uD[i].outDeg = outLinks[i];
        uD[i].rank = newRanks[i];
    }

    sortRanks(uD, 0, noUrls);
    
    // print the sorted pageranks
    for (int i = noUrls - 1; i >= 0; i--) {
        int fileNameLen = strlen(uD[i].name);
        char urlRaw[MAX_URL_LEN + 4]; 
        strcpy(urlRaw, uD[i].name);
        urlRaw[fileNameLen - 4] = '\0';
        printf("%s %.lf %.7lf\n", urlRaw, uD[i].outDeg, uD[i].rank);
    }
    
    GraphFree(urlLinks);
    fclose(fp);
}

void linksCompute(Graph urlLinks, int noUrls, double inLinks[], 
                    double outLinks[]) {
    // Retrieving outlinks for every node
    for (int i = 0; i < noUrls; i++) {
        for (int j = 0; j < noUrls; j++) {
            if (GraphIsAdjacent(urlLinks, i, j) != 0.0) {
                if (outLinks[i] == 0.5) {
                    outLinks[i] = 1.0;
                } else {
                    outLinks[i] = outLinks[i] + 1.0;
                }
            }
        }                           
    }

    // Retrieving inlinks for every node
    for (int i = 0; i < noUrls; i++) {
        for (int j = 0; j < noUrls; j++) {
            if (GraphIsAdjacent(urlLinks, j, i) != 0.0) {
                inLinks[i] = inLinks[i] + 1.0;
            }
        }
    }   
}

void weightsCompute(Graph urlLinks, int noUrls, double inLinks[], 
                    double outLinks[], Weights w[][noUrls]) {
    // Calculating wOut for each edge
    for (int i = 0; i < noUrls; i++) {
        for (int j = 0; j < noUrls; j++) {
            double wOut = 0.0;
            if (GraphIsAdjacent(urlLinks, i, j) != 0.0) {
                double oU = outLinks[j];
                double oP = 0.5;
                for (int k = 0; k < noUrls; k++) {
                    if (GraphIsAdjacent(urlLinks, i, k) != 0.0) {
                        if (oP == 0.5) {
                            oP = outLinks[k];
                        } else {
                            oP += outLinks[k];
                        }
                    }
                }
                wOut = oU/oP;
                w[i][j].wOut = wOut; 
            }
        }
    }

    // Calculating wIn for each edge
    for (int i = 0; i < noUrls; i++) {
        for (int j = 0; j < noUrls; j++) {
            double wIn = 0.0;
            if (GraphIsAdjacent(urlLinks, i, j) != 0.0) {
                double iU = inLinks[j];
                double iP = 0.0;
                for (int k = 0; k < noUrls; k++) {
                    if (GraphIsAdjacent(urlLinks, i, k) != 0.0) {
                        iP += inLinks[k];
                    }
                }
                wIn = iU/iP;
                w[i][j].wIn = wIn; 
            }
        }
    }
}

// The sorting method used is a selection sort. The following code is heavily
// inspired by the week08 lecture code: https://www.cse.unsw.edu.au/~cs2521/20T2/exercises/week08/SortLab/sorter.c
void sortRanks(urlData uD[], int lo, int hi) {
    // Sorting pages in increasing order according to their rank
    int min = 0;
    for (int i = lo; i < hi - 1; i++) {
        min = i;
        for (int j = i + 1; j < hi; j++) {
            if (uD[j].rank < uD[min].rank) {
                min = j;
            }
        }
        urlData temp;
        temp = uD[min];
        uD[min] = uD[i];
        uD[i] = temp;
    }

    // Sorting  the rank sorted array in decreasing alphabetical order in case 
    // pages share the same rank
    min = 0;
    for (int i = lo; i < hi - 1; i++) {
        min = i;
        for (int j = i + 1; j < hi; j++) {
            if (uD[j].rank == uD[min].rank) {
                if (strcmp(uD[j].name, uD[min].name) > 0) {
                    min = j;
                }
            }
        }
        urlData temp;
        temp = uD[min];
        uD[min] = uD[i];
        uD[i] = temp;
    }
}