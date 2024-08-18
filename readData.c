#include <assert.h>
#include <ctype.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "readData.h"

Graph pageGraph(FILE *collection, char mapNumtoUrl[][MAX_URL_LEN + 4], 
                int noUrls) {
    // Accounting for the appending of '.txt' by allocating 4 more indexes to
    // the maximum possible url length
    char word[MAX_URL_LEN + 4];
    
    int i = 0;
    fseek(collection, 0, SEEK_SET);
    while ((fscanf(collection, "%s", word)) != EOF) { 
        strcat(word, ".txt");
        strcpy(mapNumtoUrl[i], word);
        i++;
    }

    Graph urls = GraphNew(noUrls);
    links(urls, mapNumtoUrl);

    return urls;
}

void links(Graph urls, char numToUrl[][MAX_URL_LEN + 4]) {
    // Accounting for the appending of '.txt' by allocating 4 more indexes to
    // the maximum possible url length
    char word[MAX_URL_LEN + 4];    
    
    // Goes through each url file and makes an edge between the url and
    // the urls included in the url file's references
    for (int i = 0; i < urls->nV; i++) {
        FILE *url = fopen(numToUrl[i], "r");
        if (url == NULL) {
            fprintf(stderr, "Can't open %s\n", numToUrl[i]);
            exit(EXIT_FAILURE);
        }
        
        bool passStart = false;
        while ((fscanf(url, "%s", word)) != EOF) {
            // Based on the provided url text file format, once the string 
            // "#end" has been scanned, all the url strings have been scanned
            if (strcmp(word, "#end") == 0) {
                break;
            }
            if (passStart) {
                // This strcat is used to match the naming of files in the 
                // numToUrl array
                strcat(word, ".txt");
                for (int j = 0; j < urls->nV; j++) {
                    // The j != i makes sure that no self links are included
                    if ((strcmp(word, numToUrl[j]) == 0) && (j != i)) {
                        Edge e;
                        e.v = i; 
                        e.w = j; 
                        e.weight = 1;
                        GraphInsertEdge(urls, e);
                    }
                }
            }
            // Based on the provided url text file format, once the string 
            // "Section-1" has been scanned, the url strings are the next 
            // strings to be scanned
            if (strcmp(word, "Section-1") == 0) {
                passStart = true;
            }
        }
        fclose(url);
    }  
}