#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#define ULONG_MAX 0xFFFFFFFFUL


// Stores node index and id
typedef struct{
    unsigned long id;
    int index;
}box;

typedef struct{
    unsigned long id;  //node identification
    char *name;
    double lat, lon;  // node position
    unsigned short nsucc; //number of node successors
    unsigned long *successors;
}node;

// global declarations (no m'agraden gaire)
unsigned long nnodes = 23895681UL;
node *nodes;
box *auxiliar = NULL;
short size = 0; //size of the box


// Returns 0 if is type node, 1 if is type way and 2 if relation. -1 if none of the above
int field_type(char *line){
    if      (strcmp(line, "node") == 0)
        return 0;
    else if (strcmp(line, "way") == 0)
        return 1;
    else if (strcmp(line, "relation") == 0)
        return 2;
    else
        return -1;
}


// Returns the index from the id node.
unsigned long BinarySearch(unsigned long key, node *list, unsigned long list_len){
    unsigned long start=0UL, after_end=list_len, middle;
    unsigned long try;

    while(after_end > start){
        middle = start + ((after_end-start-1) >> 1); try = list[middle].id;
        if (key == try) return middle;
        else if ( key > try ) start=middle+1;
        else after_end=middle;
    }
    return ULONG_MAX; // if it fails set to -1 and long, instead of unsignes long
}


unsigned long LinearSearch(unsigned long key, node *list, unsigned long list_len){
    register unsigned long s;
    for(s=0; s<list_len; s++) {if (key == list[s].id) return s;}
    return ULONG_MAX;
}


// If *temp node is registered (it appears as node) is stored in auxiliar, a box-type variable.
// if node is not registered, we will skip it and link the previous to this to next to this
void ConnectingNodes(char *temp){
    long index = BinarySearch(atol(temp),nodes,nnodes);
    if (index == ULONG_MAX ) return ;

    size++;
    auxiliar = realloc(auxiliar,sizeof(box)*(size));
    auxiliar[size-1].id = atol(temp);
    auxiliar[size-1].index = index;
}


// It connects the differents nodes in the box variable auxiliar.
void ConnectingSuccesors(int oneway){
    int i;

    for(i=0; i<size-1; i++){
        nodes[auxiliar[i].index].nsucc+=1;
        nodes[auxiliar[i].index].successors = realloc(nodes[auxiliar[i].index].successors,sizeof(unsigned long)*
                nodes[auxiliar[i].index].nsucc);
        nodes[auxiliar[i].index].successors[nodes[auxiliar[i].index].nsucc-1] = auxiliar[i+1].index;

        if (oneway == 0){
            nodes[auxiliar[i+1].index].nsucc++;
            nodes[auxiliar[i+1].index].successors = realloc(nodes[auxiliar[i+1].index].successors,sizeof(unsigned long)*
                    nodes[auxiliar[i+1].index].nsucc);
            nodes[auxiliar[i+1].index].successors[nodes[auxiliar[i+1].index].nsucc-1] = auxiliar[i].index;
        }
    }

}


// registers the nodes and their parameters in a node array
void fnode(int counter, char *temp, unsigned int z){
    if(counter == 1) 		nodes[z].id  = atol(temp);
    else if (counter == 9)	nodes[z].lat = atof(temp);
    else if (counter == 10)	nodes[z].lon = atof(temp);
}


void reading(FILE *fp){
    int field=0, counter = 0;
    unsigned short oneway = 0;
    unsigned int z = 0; // cardinal

    char *line;
    size_t len = 0;
    ssize_t read;
    char *temp;

    while( (read = getline(&line, &len, fp)) != -1 ){
        // strsep (tokenizer) replaces | for 0's, "destroying" the line
        while( (temp = strsep(&line,"|")) != NULL ){
            if (counter == 0){
                field = field_type(temp);
                if (field != -1){
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

            else if(field == 1 ){
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

}


void writing(){
    char *binary_file = "data/binary.bin";
    FILE *fp;
    unsigned int i;
    unsigned long ntotnsucc=0UL;

    for(i=0; i<nnodes; i++) ntotnsucc+=nodes[i].nsucc;


    if((fp=fopen(binary_file,"wb")) == NULL)
        printf("The binary Output file can't be opened\n");

    /* Global data---header */
    if((fwrite(&nnodes,sizeof(unsigned long),1,fp)+
                fwrite(&ntotnsucc,sizeof(unsigned long),1,fp))!=2) printf("When initializing the output binary data file\n");

    /* Writting all nodes */

    if( fwrite(nodes,sizeof(node),nnodes,fp)!=nnodes) printf ("When writing nodes to the output binary data file\n");

    //Writting successors in blocks
    for (i=0; i<nnodes; i++){
        if(nodes[i].nsucc){
            if(fwrite(nodes[i].successors,sizeof(unsigned long),nodes[i].nsucc,fp)!=nodes[i].nsucc)
                printf("When writing edges to the output binary data file\n");
        }
    }
    fclose(fp);
}


int main (void){
    unsigned int i;
    time_t time;
    char *data_file = "data/spain.csv";
    FILE *fp;
    time = clock();

    if((nodes = (node*) malloc(nnodes*sizeof(node))) == NULL)
        printf("Problem allocating memory for nodes struct");

    // Initialization

    printf("Inizializing\n");
    for(i=0; i<nnodes; i++){
        nodes[i].id = 0;
        nodes[i].nsucc = 0;
        nodes[i].successors = malloc(sizeof(unsigned long)*1);
    }

    printf("Reading and Writing\n");

    fp = fopen(data_file,"r");

    if((fp=fopen(data_file,"r")) == NULL){
        printf("Error reading file\n");
        exit(EXIT_FAILURE);
    }
    else
        reading(fp);

    fclose(fp);
    writing();

    time = clock()-time;

    printf("Elapsed time: %fs \n",((double)time)/CLOCKS_PER_SEC);

    return 0;
}
