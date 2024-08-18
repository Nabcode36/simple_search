#ifndef SEARCHPAGERANK_H
#define SEARCHPAGERANK_H

#include "pageRank.h"

#define MAX_WORD_LEN 1000
#define MAX_PAGE_LIST 30
#define PAGERANK_LEN 10

// Stores information about each url's name, score and pagerank
// Score increases every time the url contains a word entered as
// a search term in the command line arguments 
typedef struct urlData {
    char name[MAX_URL_LEN + 4];
    int score;
    double rank;
} urlInfo;

// Extracts data from the files given in the directory so that the passed
// in arrays contains the correct information regarding url's pageRank and
// name. All scores will initially be set to 0
void extractUrlData(urlInfo uI[], int noUrls, FILE *urls);

// Increases the score of each url based on the number of matching words the 
// url has with the terms entered in the terminal
void urlTermMatch(urlInfo uI[], char mapNumtoTerm[][MAX_WORD_LEN], int termNo, int noUrls);

// This function sorts urls in descending order by scores first, then in descending
// order by weighted page rank, then in increasing alphabetical order by the url's name
void sortUrls(urlInfo uI[], int lo, int hi);

#endif