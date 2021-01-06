#define _USE_MATH_DEFINES
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "utils.h"
#include "A_star_aux.h"
#define R_earth 6.3781e6 //earth radius in meters
#define ULONG_MAX 0xFFFFFFFFUL


unsigned long nnodes = 23895681UL;
node *nodes;
node_complete *node_complete_variable;


void ListInicialization (ourlist *x) {
    x->start = x->end = NULL;
    x->nelems = 0;
}


/* Different heuristic functions to choose from */
double heuristic_function(node_complete *init, node_complete *prev, int method) {
    // 0. Djikstra
    if (method == 0)
        return 0.0;

    double phi1 = init->lat/180.0*M_PI;
    double phi2 = prev->lat/180.0*M_PI;
    double lambda1 = init->lon/180.0*M_PI;
    double lambda2 = prev->lon/180.0*M_PI;
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


unsigned long BinarySearch(unsigned long key, node_complete *list, unsigned long list_len) {
    unsigned long start=0UL, middle, after_end=list_len;
    unsigned long try;

    while(after_end > start) {
        middle = start + ((after_end-start - 1) >> 1);
        try = list[middle].id;
        if (key == try) return middle;
        else if (key > try) start = middle + 1;
        else after_end = middle;
    }
    return ULONG_MAX; // if it fails set to ULONG_MAX
}

//Delete nodes from list
void DeleteNode(ourlist *list, node_complete *target) {

    if (list->start == list->end) {
        list->start = list->end = NULL;
        target->prev = target-> next = NULL;
    }
    else if (target == list->start) {
        list->start = target->next;
        (target->next)->prev = NULL;
        target->next = NULL;
        target->prev = NULL;
    }
    else if (target == list->end) {
        list->end = target->prev;
        (target->prev)->next = NULL;
        target->prev = NULL;
        target->next = NULL;
    }
    else {
        (target->next) -> prev = target -> prev;
        (target->prev) -> next = target -> next ;
        target->next = target-> prev = NULL;
    }
    list->nelems--;
}

void InsertNodeAfter(ourlist *list, node_complete *target, node_complete *a_insertar) {

    if (list-> end == target) {
        list-> end = a_insertar;
        target-> next = a_insertar;
        a_insertar-> prev = target ;
        a_insertar->next = NULL;
    }
    else {
        (target-> next)-> prev = a_insertar;
        a_insertar-> prev = target ;
        a_insertar -> next =  target->next;
        target-> next = a_insertar;
    }
    list->nelems++;
}

void InsertNodeBefore(ourlist *list, node_complete *target, node_complete *a_insertar) {


    if (list->start == target) {
        target->prev = a_insertar;
        list->start = a_insertar;
        a_insertar->prev = NULL;
        a_insertar->next = target;
    }

    else {
        a_insertar-> prev = (target-> prev);
        a_insertar-> next = target ;
        (target->prev)-> next = a_insertar ;
        target-> prev = a_insertar;
    }
    list->nelems++;
}



// Add to openlist
void Add_to_Open (ourlist *list, node_complete *succesor) {

    node_complete *curr;
    int n;

    if ( list->nelems ==0 ) {
        list->start = list->end = succesor;
        succesor->prev = succesor->next = NULL;
        list->nelems++;
    }

    else {
        for ( n=0, curr = list->start; curr!=NULL ; n++, curr = curr->next) {
            //printf("id : %lu\n",curr->id);
            if ( succesor-> heuristic < curr->heuristic) { InsertNodeBefore (list, curr, succesor); return;}
        }
        InsertNodeAfter(list, list->end, succesor);
    }

}


/***********************************************************/
/*                      A* Algorithm                       */
/***********************************************************/
int A_star (unsigned int source, unsigned int goal, int method) {
    node_complete *current, *succesor ;

    int i;
    double succesor_current_cost;

    // create open list
    ourlist Open;

    //Inizializate Open list
    ListInicialization(&Open);

    Open.start = Open.end = &(node_complete_variable[source]);

    // Start g at 0
    node_complete_variable[source].g=0;

    // Compute initial value of heuristic function
    node_complete_variable[source].heuristic = heuristic_function(&(node_complete_variable[source]), &(node_complete_variable[goal]), method);

    // add 1 to number of elements
    Open.nelems++;


    while (Open.nelems>0) {

        current = Open.start ; //takes first node: the one with lowest f distance

        if (current->id == node_complete_variable[goal].id) return 0;

        for (i = 0; i < (current->nsucc); i++) {
            succesor = &node_complete_variable[*(current->successors+i)];
            succesor_current_cost = current->g+heuristic_function(succesor, current, method);
            if (succesor->list == 1) {
                if (succesor->g <= succesor_current_cost) continue;
                DeleteNode(&Open, succesor); // delete node to update its position
            }

            else if (succesor->list == 2) {
                if (succesor->g <= succesor_current_cost) continue;
                succesor->list = 1;
            }

            else
                succesor->list = 1;

            succesor->g = succesor_current_cost;
            succesor->heuristic = succesor->g + heuristic_function(succesor, &(node_complete_variable[goal]), method);
            Add_to_Open ( &Open, succesor);
            succesor-> parent = current;
        }
        current->list = 2;
        DeleteNode (&Open, current);
    }
    return 1;
}



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
