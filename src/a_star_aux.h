#ifndef A_STAR_AUX_H_INCLUDED
#define A_STAR_AUX_H_INCLUDED

/* Shared definitions */

typedef struct node2 {
	unsigned long id;
	double lon, lat;
	unsigned short nsucc;
	unsigned long *successors;
	struct node2 *prev, *next,*parent; // next and prev for the open list structure. parent to know the final path
	char which_list; // Which list it belongsto: 0. none, 1. open, 2. closed
	double heuristic; // heuristic distance + g distance
	double g; // g distance from node_start
}node_dist;


/************************************************************************/
/*      Functions defined in functions.c to be used in main.c           */
/************************************************************************/

extern void NodeInit(unsigned int i);

extern double heuristic_function(node_dist *init, node_dist *prev, int method);


 // returns the node index from the node id
extern unsigned long BinarySearch(unsigned long key, node_dist *list, unsigned long list_len);


extern int A_star (unsigned int node_start, unsigned int node_goal, int method);

extern void Reading_csv(char *binary_file);

#endif
