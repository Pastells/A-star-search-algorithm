#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "A_star_aux.h"

#define INFTY 1000000000

node *nodes;
node_complete *node_complete_variable;


int main (int argc, char **argv){
    //Declarations
    unsigned int i;
    time_t time_A; // A* function time
    time_t time_reading; // reading function time
    time_t time_total; // total time of code
    char *heuristics[] = {"Dijkstra", "Haversine", "Sperical law of cosines", "Equirectangular aprox"};
    char *path_file= "results/optimal_path.csv";

    time_total = clock();

    unsigned long nnodes = 23895681UL;

    node_complete_variable = malloc ( sizeof(node_complete)*nnodes);

    int source, goal;
    int heuristic_number = 1;
    int source_id = 240949599;
    int goal_id = 195977239;

    if (argc>1) {heuristic_number = atoi(argv[1]);}
    if (argc>2) {source_id        = atoi(argv[2]);}
    if (argc>3) {goal_id          = atoi(argv[3]);}

    if (heuristic_number > 3){
        printf("Heuristic number %d does not exist\n", heuristic_number);
        exit(EXIT_FAILURE);
    }

    time_reading = clock();

    //Read file
    reading_from_file();

    time_reading = clock()-time_reading;


    //Inizialization
    for (i = 0; i < nnodes; i++){
        node_complete_variable[i].lon = nodes[i].lon;
        node_complete_variable[i].lat = nodes[i].lat;
        node_complete_variable[i].nsucc = nodes[i].nsucc;
        node_complete_variable[i].successors = nodes[i].successors;
        node_complete_variable[i].g = INFTY;
        node_complete_variable[i].list = 0;
        node_complete_variable[i].heuristic = INFTY;
        node_complete_variable[i].id = nodes[i].id;
        node_complete_variable[i].parent = NULL;
    }

    free(nodes);

    // Find index of source and goal
    source= BinarySearch(source_id);
    goal = BinarySearch(goal_id);

    printf("Calculating using %s heuristic\n", heuristics[heuristic_number]);

    time_A=clock();

    if (A_star(source, goal, heuristic_number) == 0){

        time_A=clock()-time_A;
        time_total = clock() - time_total;

        node_complete *curr;
        long n;
        FILE *fp;

        fp=fopen(path_file, "w");
        fprintf(fp, "# Path from GOAL to SOURCE\n");
        fprintf(fp, "# Time elapsed by A* Algorithm %fs \n", ((double)time_A)/CLOCKS_PER_SEC);
        fprintf(fp, "# Time elapsed reading binary file %fs\n", ((double)time_reading)/CLOCKS_PER_SEC);
        fprintf(fp, "# Total time elapsed %fs\n", ((double)time_total)/CLOCKS_PER_SEC);
        fprintf(fp, "# Iteration, Node Id, Distance, latitude, longitude\n");

        for ( n = 0, curr= &(node_complete_variable[goal]); curr; n++, curr=curr->parent)
            fprintf(fp, "%4.0ld, %10.0lu, %6.2f, %2.7f, %2.7f\n", n, curr->id, curr->g, curr->lat, curr->lon);
        fclose(fp);

        printf("Time elapsed by A* Algorithm %fs\n", ((double)time_A)/CLOCKS_PER_SEC );
    }

    else {printf("Error, no path from source to goal\n"); time_total = clock() - time_total;}

    printf("Total time spent : %fs \nReading function from binary file time: %fs\n",
            ((double)time_total)/CLOCKS_PER_SEC, ((double)time_reading)/CLOCKS_PER_SEC);

    return 0;
}
