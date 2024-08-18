// COMP2521 Assignment 2

// Written by: Nabeel Pathan (z5359531) 
// Date: 14/11/2022

#include <assert.h>
#include <ctype.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "searchPageRank.h"

int main(int argc, char *argv[]) {
    // Prints error if there is not at least one search term provided
    if (argc < 2) {
        fprintf(stderr, "Usage: %s term0 term1 term2 ....\n",
                argv[0]);
        return EXIT_FAILURE;
    }

    // Creating an array which maps an array index to the words 
    // entered in the terminal
    int termNo = argc - 1;
    char mapNumtoTerm[argc - 1][MAX_WORD_LEN];
    for (int i = 1; i < argc; i++) {
        strcpy(mapNumtoTerm[i - 1], argv[i]);
    }

    // Extracting the number of urls
    FILE *fp = fopen("pageRankList.txt", "r");
    if (fp == NULL) {
        fprintf(stderr, "Can't open pageRankList.txt\n");
        exit(EXIT_FAILURE);
    }

    // Finding the number of urls based on the number of lines
    // in pageRankList.txt
    char c;
    int noUrls = 0;
    while ((c = fgetc(fp)) != EOF) {
        if (c == '\n') {
            noUrls++;
        }
    }
    
    urlInfo uI[noUrls];
    extractUrlData(uI, noUrls, fp);
    urlTermMatch(uI, mapNumtoTerm, termNo, noUrls);
    sortUrls(uI, 0, noUrls);

    // Print the top 30 pages according to the number of matching terms,
    // then page rank, then name. Pages with no matching terms are not
    // printed. If the all urls has been scanned before the 30 page limit,
    // stop printing as well
    int i = 0;
    while (i < MAX_PAGE_LIST && i != noUrls) {
        if (uI[i].score > 0) {
            printf("%s\n", uI[i].name);
        }
        i++;
    }

    fclose(fp);
}

void extractUrlData(urlInfo uI[], int noUrls, FILE *urls) {
    fseek(urls, 0, SEEK_SET);
    for (int i = 0; i < noUrls; i++) {
        // The amount of space allocated to each read in line is based on the 
        // standard format of pageRankList.txt
        int maxLinelength = MAX_URL_LEN + noUrls + PAGERANK_LEN + 1;
        char line[maxLinelength];
        fgets(line, maxLinelength, urls);
        for (int j = 0; line[j] != ' '; j++) {
            uI[i].name[j] = line[j];   
            if (line[j + 1] == ' ') {
                uI[i].name[j + 1] = '\0';
            }
        }
    }

    // Initialising all the page/url scores to be 0
    for (int i = 0; i < noUrls; i++) {
        uI[i].score = 0;
    }

    // Extracting the page ranks
    fseek(urls, 0, SEEK_SET);
    for (int i = 0; i < noUrls; i++) {
        int maxLinelength = MAX_URL_LEN + noUrls + PAGERANK_LEN + 1;
        char line[maxLinelength];
        fgets(line, maxLinelength, urls);
        int spaceCount = 0;
        
        // This setup is to make an array that will store the rank string, which 
        // will be converted to a double
        int numIndex = 0;
        char num[PAGERANK_LEN];
        char *eptr;
        for (int j = 0; line[j] != '\n'; j++) {
            // Based on the given format of pageRankList.txt, once 2 space
            // characters have been passed, the file pointer will be up to
            // url rank
            if (spaceCount == 2) {
                num[numIndex] = line[j];
                numIndex++;
            }
            if (line[j] == ' ') {
                spaceCount++;
            }
        }
        uI[i].rank = strtod(num, &eptr);
    }
}

void urlTermMatch(urlInfo uI[], char mapNumtoTerm[][MAX_WORD_LEN], int termNo, 
                    int noUrls) {
    FILE *Index = fopen("invertedIndex.txt", "r");
    
    // For every term entered in the command line, check for the urls that 
    // contain the term and update their scores
    for (int i = 0; i < termNo; i++) {
        fseek(Index, 0, SEEK_SET);
        int maxLinelength = MAX_URL_LEN + noUrls + PAGERANK_LEN + 1;
        char line[maxLinelength];
        // Goes through each line of invertedIndex.txt until the line containing
        // the current iteration's term is found
        while (fgets(line, maxLinelength, Index) != NULL) {
            char word[MAX_WORD_LEN];
            // Saves the term in the current line, to be compared to the current
            // iteration's term
            sscanf(line, "%s", word);    
            if (strcmp(mapNumtoTerm[i], word) == 0) {
                int wordLen = strlen(word);
                char url[MAX_URL_LEN];
                int urlIndex = 0;
                // Goes through each url character by character and resets the 
                // url string once it updates the corresponding url's score
                for (int j = wordLen + 1; line[j] != '\n'; j++) {
                    if (line[j] != ' ') {
                        url[urlIndex] = line[j];
                        urlIndex++;
                    }
                    // These are the cases where the whole url name has been 
                    // scanned in. Reset the url array
                    if (line[j] == ' ' || line[j + 1] == '\n' || 
                        line[j + 1] == '\0') {
                        url[urlIndex] = '\0';
                        urlIndex = 0;
                        // Looks for the matching url in the uI struct and 
                        // updates its score
                        for (int k = 0; k < noUrls; k++) {
                            char name[MAX_URL_LEN];
                            strcpy(name, uI[k].name);
                            if (strcmp(url, name) == 0) {
                                uI[k].score++;
                            }
                        }   
                    }
                }
                // No more lines need to be scanned in once the line containing
                // the current iteration's term has been found. Prevents 
                // unnecssary scans
                break;
            }
        }
    }
    
    fclose(Index);
}

// The sorting method used is a selection sort. The following code is heavily
// inspired by the week08 lecture code: https://www.cse.unsw.edu.au/~cs2521/20T2/exercises/week08/SortLab/sorter.c
void sortUrls(urlInfo uI[], int lo, int hi) {
    // Sorting pages in descending order according to their score
    int min = 0;
    for (int i = lo; i < hi - 1; i++) {
        min = i;
        for (int j = i + 1; j < hi; j++) {
            if (uI[j].score > uI[min].score) {
                min = j;
            }
        }
        urlInfo temp;
        temp = uI[min];
        uI[min] = uI[i];
        uI[i] = temp;
    }

    // Sorting pages with equal scores in descending order according 
    // to their weighted page rank
    min = 0;
    for (int i = lo; i < hi - 1; i++) {
        min = i;
        for (int j = i + 1; j < hi; j++) {
            if (uI[j].score == uI[min].score) {
                if (uI[j].rank > uI[min].rank) {
                    min = j;
                }
            }
        }
        urlInfo temp;
        temp = uI[min];
        uI[min] = uI[i];
        uI[i] = temp;
    }

    // Sorting pages with equal scores and ranks in increasing alphabetical 
    // order according to their weighted page rank
    min = 0;
    for (int i = lo; i < hi - 1; i++) {
        min = i;
        for (int j = i + 1; j < hi; j++) {
            if (uI[j].score == uI[min].score && uI[j].rank == uI[min].rank) {
                if (strcmp(uI[j].name, uI[min].name) < 0) {
                    min = j;
                }
            }
        }
        urlInfo temp;
        temp = uI[min];
        uI[min] = uI[i];
        uI[i] = temp;
    }
}