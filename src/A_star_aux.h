#ifndef A_STAR_AUX_H_INCLUDED
#define A_STAR_AUX_H_INCLUDED

/* Shared definitions */

typedef struct node2 {
	unsigned long id;
	double lat, lon;
	unsigned short nsucc;
	unsigned long *successors;
	struct node2 *prev, *next,*parent; // next and prev for the open list structure. parent to know the final path
	char list; // 0: nothing, 1: open, 2: closed
	double heuristic; // heuristic distance+ g distance
	double g; // g distance: from source to this node
}node_complete;


typedef struct {
	node_complete * start, *end;
	unsigned long nelems; // number of elements on list
} ourlist; // open list structure


/************************************************************************/
/*      Functions defined in functions.c to be used in main.c           */
/************************************************************************/

extern void ListInicialization ( ourlist *x);

extern double heuristic_function(node_complete *init, node_complete *prev, int method);


 // returns the node index from the node id
extern unsigned long BinarySearch(unsigned long key, node_complete *list, unsigned long list_len);

/************************************************************************/
/*                   Utility functions for linked list                  */
/************************************************************************/
extern void DeleteNode(ourlist *list, node_complete *target);
extern void InsertNodeAfter(ourlist *list, node_complete *target , node_complete *a_insertar);
extern void InsertNodeBefore(ourlist *list, node_complete *target , node_complete *a_insertar);
extern void Add_to_Open(ourlist *list, node_complete *succesor);
/*****************************************************************/


extern int A_star (unsigned int source, unsigned int goal, int method);


extern void Reading_csv(char *binary_file);

#endif
