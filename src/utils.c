#include <stdio.h>
#include <stdlib.h>

// Handle errors
void ExitError(const char *miss, int errcode) {
    fprintf(stderr, "\nERROR: %s.\nStopping...\n\n", miss);
    exit(errcode);
}

// Structure to conain node information
typedef struct {
    unsigned long id;  //node identification
    double lat, lon;  // node position
    unsigned short nsucc; //number of node successors
    unsigned long *successors;
}node;
