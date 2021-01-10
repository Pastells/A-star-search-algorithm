/* Functions defined in a_star_aux.c to be used in main.c */

#ifndef A_STAR_AUX_H_INCLUDED
#define A_STAR_AUX_H_INCLUDED

#include "utils.h" // node_dist struct

extern void Reading_csv(char *binary_file);
extern void NodeInit(unsigned int i);
extern unsigned long BinarySearch(unsigned long key, node_dist *list, unsigned long list_len);
extern int A_star(unsigned int node_start, unsigned int node_goal, int method);

#endif
