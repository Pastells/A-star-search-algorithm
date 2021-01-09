/* Function declarations for a_star main program */

#define _USE_MATH_DEFINES
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "utils.h"
#include "a_star_aux.h"
#define R_earth 6.378e6 // Earth radius (m)
#define ULONG_MAX 0xFFFFFFFFUL
#define INFTY 1000000000

/*************************************************************************/

unsigned long nnodes = 23895681UL;
node *nodes;
node_dist *node_dist_var;

/* Initialize node in node_dist_var
   Uses nodes and node_dist_var as global variables */
void NodeInit(unsigned int i) {
    node_dist_var[i].lon = nodes[i].lon;
    node_dist_var[i].lat = nodes[i].lat;
    node_dist_var[i].nsucc = nodes[i].nsucc;
    node_dist_var[i].successors = nodes[i].successors;
    node_dist_var[i].g = INFTY;
    node_dist_var[i].which_list = 0;
    node_dist_var[i].heuristic = INFTY;
    node_dist_var[i].id = nodes[i].id;
    node_dist_var[i].parent = NULL;
}


/* Different heuristic functions to choose from */
double heuristic_function(node_dist *init, node_dist *prev, int method) {
    // 0. Djikstra
    if (method == 0)
        return 0.0;

    double lambda1 = init->lon/180.0*M_PI;
    double lambda2 = prev->lon/180.0*M_PI;
    double phi1 = init->lat/180.0*M_PI;
    double phi2 = prev->lat/180.0*M_PI;
    double lat_dif = fabs(phi1-phi2);
    double long_dif = fabs(lambda1-lambda2);

    // 1. Haversine formula
    if (method == 1) {
        double a = sin(lat_dif/2)*sin(lat_dif/2) + cos(phi1)*cos(phi2)*sin(long_dif/2)*sin(long_dif/2);
        double c = 2*atan2(sqrt(a), sqrt(1-a));
        return c*R_earth;
    }

    // 2. Spherical law of cosines
    else if (method == 2) {
        return acos( sin(phi1)*sin(phi2)+cos(phi1)*cos(phi2)*cos(long_dif) ) * R_earth;
    }

    // 3. Equirectangular approximation
    else {
        double x = long_dif * cos( (phi1+phi2)/2 );
        return R_earth*sqrt(x*x+lat_dif*lat_dif);
    }

}


/*************************************************************************/


/* Returns node index from the node id
   Same as in create_binary.c, except for *list being a node_dist struct instead of node */
unsigned long BinarySearch(unsigned long key, node_dist *list, unsigned long list_len) {
    unsigned long start=0UL, middle, after_end=list_len, try;

    while(after_end > start) {
        middle = start + ((after_end-start - 1) >> 1);
        try = list[middle].id;
        if (key == try)
            return middle;
        else if (key > try)
            start = middle + 1;
        else
            after_end = middle;
    }
    return ULONG_MAX; // if it fails set to ULONG_MAX
}


/***********************************************************/
/*                      Reading                            */
/***********************************************************/

void Reading_csv(char *binary_file) {
    unsigned int i;
    static unsigned long *allsuccesors;
    unsigned long ntotnsucc=0UL;
    FILE *fin;

    fin=fopen(binary_file, "rb");

    // checks for possible errors
    if ( (fread(&nnodes, sizeof(unsigned long), 1, fin) +
          fread(&ntotnsucc, sizeof(unsigned long), 1, fin)) !=2 )
        ExitError("reading the header of binary file", 101);

    if ((nodes=malloc(sizeof(node)*nnodes)) == NULL)
        ExitError("allocating memory for nodes vector", 102);

    if((allsuccesors=malloc(sizeof(unsigned long)*ntotnsucc))==NULL)
        ExitError("allocating memory for edges vector", 103);

    if (fread(nodes, sizeof(node), nnodes, fin)!=nnodes)
        ExitError("reading nodes from binary file", 104);

    if (fread(allsuccesors, sizeof(unsigned long), ntotnsucc, fin)!=ntotnsucc)
        ExitError("reading successors from binary file", 105);

    fclose(fin);

    for (i=0;i<nnodes; i++) if (nodes[i].nsucc) {
        nodes[i].successors=allsuccesors;
        allsuccesors+=nodes[i].nsucc;
    }
}


/***********************************************************/
/*                         Open List                       */
/***********************************************************/

/* struct to represent Open List */
typedef struct {
	node_dist * start, *end;
	unsigned long nelems; // number of elements on list
} open_list;


/* Initialize open list */
void ListInit(open_list *list) {
    list->start = list->end =  NULL;
    list->nelems = 0;
}


void InsertNodeAfter(open_list *list, node_dist *target, node_dist *node) {
    if (list->end == target) {
        list->end = node;
        target->next = node;
        node->prev = target ;
        node->next = NULL;
    }
    else {
        (target->next)->prev = node;
        node->prev = target ;
        node->next =  target->next;
        target->next = node;
    }
    list->nelems++;
}


void InsertNodeBefore(open_list *list, node_dist *target, node_dist *node) {
    if (list->start == target) {
        target->prev = node;
        list->start = node;
        node->prev = NULL;
        node->next = target;
    }

    else {
        node->prev = (target->prev);
        node->next = target ;
        (target->prev)->next = node;
        target->prev = node;
    }
    list->nelems++;
}


// Add to openlist
void AddNode (open_list *list, node_dist *succesor) {
    node_dist *curr;
    int n;

    if (list->nelems == 0) {
        list->start = list->end = succesor;
        succesor->prev = succesor->next = NULL;
        list->nelems++;
    }

    else {
        for (n=0, curr = list->start; curr!=NULL ; n++, curr = curr->next) {
            if (succesor->heuristic < curr->heuristic) {
                InsertNodeBefore (list, curr, succesor);
                return;
            }
        }
        InsertNodeAfter(list, list->end, succesor);
    }
}


//Delete node from open_list
void DeleteNode(open_list *list, node_dist *target) {

    if (list->start == list->end) {
        list->start = list->end = NULL;
        target->prev = target->next = NULL;
    }
    else if (target == list->start) {
        list->start = target->next;
        (target->next)->prev = NULL;
        target->next = target->prev = NULL;
    }
    else if (target == list->end) {
        list->end = target->prev;
        (target->prev)->next = NULL;
        target->next = target->prev = NULL;
    }
    else {
        (target->next)->prev = target->prev;
        (target->prev)->next = target->next ;
        target->next = target->prev = NULL;
    }
    list->nelems--;
}


/***********************************************************/
/*                      A* Algorithm                       */
/***********************************************************/
int A_star (unsigned int node_start, unsigned int node_goal, int method) {
    node_dist *current, *succesor ;
    unsigned int i;
    double succesor_current_cost;
    open_list open;

    /* Inizializate Open list */
    ListInit(&open);

    /* Put node_start in the open list with f=h (e.i. g=0) */
    open.start = open.end = &(node_dist_var[node_start]);
    node_dist_var[node_start].g=0;
    /* Initialize heuristic function */
    node_dist_var[node_start].heuristic = heuristic_function(&(node_dist_var[node_start]), &(node_dist_var[node_goal]), method);
    /* set nelems to one */
    open.nelems++;


    while (open.nelems>0) {

        /* set current to node with lowest f distance in open list */
        current = open.start ;

        /* Check if we found a solution, return 0 */
        if (current->id == node_dist_var[node_goal].id)
            return 0;

        /* for all successors of current  node */
        for (i = 0; i < (current->nsucc); i++) {
            succesor = &node_dist_var[*(current->successors+i)];

            /* set successor cost to g(current) + h(successor, current) */
            succesor_current_cost = current->g+heuristic_function(succesor, current, method);

            /* case in open list */
            if (succesor->which_list == 1) {
                if (succesor->g <= succesor_current_cost) continue;
                /* delete node to update its position in open list */
                DeleteNode(&open, succesor);
            }

            /* case in closed list */
            else if (succesor->which_list == 2) {
                if (succesor->g <= succesor_current_cost) continue;
                /* move to open list */
                succesor->which_list = 1;
            }

            else
                /* move to open list */
                succesor->which_list = 1;

            /* set g(successor) to its current cost and update heuristic */
            succesor->g = succesor_current_cost;
            succesor->heuristic = succesor->g + heuristic_function(succesor, &(node_dist_var[node_goal]), method);
            AddNode ( &open, succesor);
            /* set the parent of succesor to current */
            succesor->parent = current;
        }
        /* move current to closed list */
        current->which_list = 2;
        DeleteNode (&open, current);
    }
    /* if algorithm ends without finding the goal return 1 */
    return 1;
}
