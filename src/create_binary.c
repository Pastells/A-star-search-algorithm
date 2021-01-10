/* Read csv file with nodes, ways and relations and create a binary file
   to be used during the A* algorithm execution
   Pol Pastells, Narcís Font, January 2021 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include "utils.h" // ExitError, node and node_dist structs
#define ULONG_MAX 0xFFFFFFFFUL

/***********************************************************/
/*              Funciton declarations                      */
/***********************************************************/


// Stores node index and id
typedef struct{
    unsigned long id;
    int index;
}box;


// global declarations
unsigned long nnodes = 23895681UL;
node *nodes;
box *aux_box = NULL;
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



void LinkNodes(char *node_temp) {
    long index;

    /* If node_temp is not registered, skip. Previous and next nodes will be connected */
    index = BinarySearch(atol(node_temp), nodes, nnodes);
    if (index == ULONG_MAX)
        return ;

    /* If *node_temp is registered, store in aux_box */
    // Size is a global variable
    size++;
    aux_box = realloc(aux_box, sizeof(box)*(size));
    aux_box[size-1].index = index;
    aux_box[size-1].id = atol(node_temp);
}


// It connects the differents nodes in the box variable aux_box.
void LinkSuccesors(int oneway) {
    short i;

    // Size is a global variable
    for(i=0; i<size-1; i++) {
        nodes[aux_box[i].index].nsuccdim+=1;
        nodes[aux_box[i].index].successors = realloc(nodes[aux_box[i].index].successors,sizeof(unsigned long)*
                nodes[aux_box[i].index].nsuccdim);
        nodes[aux_box[i].index].successors[nodes[aux_box[i].index].nsuccdim-1] = aux_box[i+1].index;

        if (oneway == 0) {
            nodes[aux_box[i+1].index].nsuccdim++;
            nodes[aux_box[i+1].index].successors = realloc(nodes[aux_box[i+1].index].successors,sizeof(unsigned long)*
                    nodes[aux_box[i+1].index].nsuccdim);
            nodes[aux_box[i+1].index].successors[nodes[aux_box[i+1].index].nsuccdim-1] = aux_box[i].index;
        }
    }
}


// fill id, longitude and latitude for node i
void FillNode(int column, char *node_temp, unsigned int i) {
    if(column == 1)
        nodes[i].id  = atol(node_temp);
    else if (column == 9)
        nodes[i].lat = atof(node_temp);
    else if (column == 10)
        nodes[i].lon = atof(node_temp);

    /* else pass */
}


/***********************************************************/
/*                  Reading and Writing                    */
/***********************************************************/

/* Read csv file with specific format */
void Reading_csv(char *data_file) {
    int field = 0, column = 0;
    unsigned short oneway = 0;
    unsigned int i = 0;
    FILE *fin;
    char *line;
    size_t len = 0;
    ssize_t read;
    char *node_temp;

    if ((fin=fopen(data_file,"r")) == NULL)
            ExitError("The data file does not exist or cannot be opened", 1);

    /* Read line by line as a string */
    while( (read = getline(&line, &len, fin)) != -1 ) {

        // strsep (tokenizer) replaces | for 0's, "destroying" the line
        while( (node_temp = strsep(&line,"|")) != NULL ) {
            if (column == 0) {
                field = field_type(node_temp);
                if (field != -1) {
                    // Size is a global variable
                    if(size > 1)
                        LinkSuccesors(oneway);
                    free(aux_box);
                    aux_box = NULL;
                    size = 0;
                    oneway = 0;
                }
            }

            if (field == 0 )
                FillNode(column, node_temp, i);

            else if(field == 1) {
                if (column == 7 && strcmp(node_temp,"oneway") == 0)
                    oneway=1;
                if (column>=9)
                    LinkNodes(node_temp);
            }
            else if (field == -1)
                LinkNodes(node_temp);
            column++;
        }
        i++;
        column = 0;
    }
    fclose(fin);
}


void Writing_bin(char *binary_file) {
    FILE *fin;
    unsigned int i;

    /* Computing the total number of successors */
    unsigned long ntotnsuccdim=0UL;
    for(i=0; i<nnodes; i++)
        ntotnsuccdim+=nodes[i].nsuccdim;

    /* Open binary file to write into */
    if((fin=fopen(binary_file,"wb")) == NULL)
        ExitError("The binary Ouput file does not exist or cannot be opened", 2);

    /* Global data --- header */
    if( fwrite(&nnodes, sizeof(unsigned long), 1,fin) +
        fwrite(&ntotnsuccdim, sizeof(unsigned long), 1,fin) !=2)
        ExitError("When initializing the output binary data file", 3);

    /* Writting all nodes */
    if( fwrite(nodes, sizeof(node), nnodes, fin) != nnodes)
        ExitError("When writing nodes to the output binary data file", 4);

    /* Writting successors in blocks */
    for (i=0; i<nnodes; i++) if(nodes[i].nsuccdim) {
            if( fwrite(nodes[i].successors, sizeof(unsigned long), nodes[i].nsuccdim,fin) !=
                nodes[i].nsuccdim)
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
        nodes[i].nsuccdim = 0;
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
