/* Read csv file with nodes, ways and relations and create a binary file
   to be used during the A* algorithm execution
   Pol Pastells, Narcís Font, January 2021 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include "utils.h"
#define ULONG_MAX 0xFFFFFFFFUL

/***********************************************************/
/*              Funciton declarations                      */
/***********************************************************/


// Stores node index and id
typedef struct{
    unsigned long id;
    int index;
}box;


// global declarations (no m'agraden gaire)
unsigned long nnodes = 23895681UL;
node *nodes;
box *auxiliar = NULL;
short size = 0; //size of the box


/* Function to classify lines by 0. node, 1. way, 2. relation, -1. other*/
int field_type(char *line) {
    if      (strcmp(line, "node") == 0)
        return 0;
    else if (strcmp(line, "way") == 0)
        return 1;
    else if (strcmp(line, "relation") == 0)
        return 2;
    else
        return -1;
}


// Returns node index from the node id
unsigned long BinarySearch(unsigned long key, node *list, unsigned long list_len) {
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


// No es fa servir, guardar a algun lloc i borrar-lo d'aquí
unsigned long LinearSearch(unsigned long key, node *list, unsigned long list_len) {
    register unsigned long s;
    for(s=0; s<list_len; s++) {if (key == list[s].id) return s;}
    return ULONG_MAX;
}


/* If *temp node is registered (it appears as node) is stored in auxiliar, a box-type variable.
*  if node is not registered, we will skip it and link the previous to this to next to this */
void ConnectingNodes(char *temp) {
    long index = BinarySearch(atol(temp),nodes,nnodes);
    if (index == ULONG_MAX) return ;

    // Size is a global variable
    size++;
    auxiliar = realloc(auxiliar,sizeof(box)*(size));
    auxiliar[size-1].id = atol(temp);
    auxiliar[size-1].index = index;
}


// It connects the differents nodes in the box variable auxiliar.
void ConnectingSuccesors(int oneway) {
    short i;

    // Size is a global variable
    for(i=0; i<size-1; i++) {
        nodes[auxiliar[i].index].nsucc+=1;
        nodes[auxiliar[i].index].successors = realloc(nodes[auxiliar[i].index].successors,sizeof(unsigned long)*
                nodes[auxiliar[i].index].nsucc);
        nodes[auxiliar[i].index].successors[nodes[auxiliar[i].index].nsucc-1] = auxiliar[i+1].index;

        if (oneway == 0) {
            nodes[auxiliar[i+1].index].nsucc++;
            nodes[auxiliar[i+1].index].successors = realloc(nodes[auxiliar[i+1].index].successors,sizeof(unsigned long)*
                    nodes[auxiliar[i+1].index].nsucc);
            nodes[auxiliar[i+1].index].successors[nodes[auxiliar[i+1].index].nsucc-1] = auxiliar[i].index;
        }
    }
}


// fill id, longitude and latitude for node z
void fnode(int counter, char *temp, unsigned int z) {
    if(counter == 1) 		nodes[z].id  = atol(temp);
    else if (counter == 9)	nodes[z].lat = atof(temp);
    else if (counter == 10)	nodes[z].lon = atof(temp);
}


/***********************************************************/
/*                  Reading and Writing                    */
/***********************************************************/

void Reading_csv(char *data_file) {
    int field=0, counter = 0;
    unsigned short oneway = 0;
    unsigned int z = 0; // cardinal
    FILE *fin;

    char *line;
    size_t len = 0;
    ssize_t read;
    char *temp;

    if ((fin=fopen(data_file,"r")) == NULL)
            ExitError("The data file does not exist or cannot be opened", 1);

    while( (read = getline(&line, &len, fin)) != -1 ) {
        // strsep (tokenizer) replaces | for 0's, "destroying" the line
        while( (temp = strsep(&line,"|")) != NULL ) {
            if (counter == 0) {
                field = field_type(temp);
                if (field != -1) {
                    // Size is a global variable
                    if(size > 1)
                        ConnectingSuccesors(oneway);
                    free(auxiliar);
                    auxiliar = NULL;
                    size = 0;
                    oneway = 0;
                }
            }

            if (field == 0 )
                fnode(counter, temp, z);

            else if(field == 1 ) {
                if (counter == 7 && strcmp(temp,"oneway") == 0)
                    oneway=1;
                if (counter>=9)
                    ConnectingNodes(temp);
            }
            else if (field == -1)
                ConnectingNodes(temp);
            counter++;
        }
        z++;
        counter = 0;
    }
    fclose(fin);
}


void Writing_bin(char *binary_file) {
    FILE *fin;
    unsigned int i;

    /* Computing the total number of successors */
    unsigned long ntotnsucc=0UL;
    for(i=0; i<nnodes; i++)
        ntotnsucc+=nodes[i].nsucc;

    /* Open binary file to write into */
    if((fin=fopen(binary_file,"wb")) == NULL)
        ExitError("The binary Ouput file does not exist or cannot be opened", 2);

    /* Global data --- header */
    if( fwrite(&nnodes, sizeof(unsigned long), 1,fin) +
        fwrite(&ntotnsucc, sizeof(unsigned long), 1,fin) !=2)
        ExitError("When initializing the output binary data file", 3);

    /* Writting all nodes */
    if( fwrite(nodes, sizeof(node), nnodes, fin) != nnodes)
        ExitError("When writing nodes to the output binary data file", 4);

    /* Writting successors in blocks */
    for (i=0; i<nnodes; i++) if(nodes[i].nsucc) {
            if( fwrite(nodes[i].successors, sizeof(unsigned long), nodes[i].nsucc,fin) !=
                nodes[i].nsucc)
                ExitError("When writing edges to the output binary data file", 5);
    }
    fclose(fin);
}


/***********************************************************/
/*                     Main program                        */
/***********************************************************/

int main (int argc, char *argv[]) {
    unsigned int i;
    time_t time;
    time = clock();

    /* Default files, can be overwritten with two first arguments*/
    char *data_file = "data/spain.csv";
    char *binary_file = "data/binary.bin";

    if (argc>1) {data_file   = argv[1];}
    if (argc>2) {binary_file = argv[2];}

    if((nodes = (node*) malloc(nnodes*sizeof(node))) == NULL)
        ExitError("Allocating memory for nodes struct",6);

    // Initialization

    printf("Inizializing\n");
    for(i=0; i<nnodes; i++) {
        nodes[i].id = 0;
        nodes[i].nsucc = 0;
        nodes[i].successors = malloc(sizeof(unsigned long)*1);
    }

    printf("Reading csv file\n");
    Reading_csv(data_file);

    printf("Writing binary file\n");
    Writing_bin(binary_file);

    time = clock()-time;
    printf("Elapsed time: %fs \n",((double)time)/CLOCKS_PER_SEC);

    return 0;
}
