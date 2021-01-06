#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

extern void ExitError(const char *miss, int errcode);

// Structure to conain node information
typedef struct {
    unsigned long id;  //node identification
    double lat, lon;  // node position
    unsigned short nsucc; //number of node successors
    unsigned long *successors;
}node;

#endif
