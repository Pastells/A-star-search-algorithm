/* Function ExitErro defined in utils.c used in all programs
   node and node_dist stucts */

#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

extern void ExitError(const char *miss, int errcode);

// Structure to conain node information
typedef struct {
    unsigned long id;  //node identification
    double lon, lat;  // node position
    unsigned short nsuccdim; //dimension os successors
    unsigned long *successors;
}node;


/* Struct with node, it's neighbors and distances */
typedef struct _node {
    // node struct {
	    unsigned long id;
	    double lon, lat;
        unsigned short nsuccdim; //dimension os successors
	    unsigned long *successors;
    // }

    /* prev, next and parent are node struct, like the one in utils.c
       prev and next are for the open list struct
       while parent is used to recover the resulting path */
	struct _node *prev, *next,*parent;

    /* Which list it belongs to: 0. none, 1. open, 2. closed */
	char which_list;
	double heuristic; // heuristic distance + g distance
	double g; // g distance from node_start
} node_dist;

#endif
