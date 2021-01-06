/* Call A_star algorithm with a heuristic of the heuristics_list
   and write result to file
   Pol Pastells, Narcís Font, January 2021 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "utils.h"
#include "A_star_aux.h"
#define INFTY 1000000000


node *nodes;
node_complete *node_complete_variable;


int main (int argc, char *argv[]) {
    //Declarations
    unsigned int i;
    unsigned long nnodes = 23895681UL;
    char *heuristics_list[] = {"Dijkstra", "Haversine", "Sperical law of cosines", "Equirectangular aprox"};
    char *path_file= "results/optimal_path.csv";
    int source, goal;

    time_t time_A; // A* function time
    time_t time_reading; // reading function time
    time_t time_total; // total time of code
    time_total = clock();

    /* Default options, can be overwritten with four first arguments*/
    int heuristic_number = 1;
    int source_id = 240949599;
    int goal_id = 195977239;
    char *binary_file = "data/binary.bin";

    if (argc>1) {heuristic_number = atoi(argv[1]);}
    if (argc>2) {source_id        = atoi(argv[2]);}
    if (argc>3) {goal_id          = atoi(argv[3]);}
    if (argc>4) {binary_file      =      argv[4];}

    if (heuristic_number > 3) {
        printf("Heuristic number %d does not exist\n", heuristic_number);
        exit(EXIT_FAILURE);
    }

    node_complete_variable = malloc ( sizeof(node_complete)*nnodes);

    time_reading = clock();

    //Read file
    Reading_csv(binary_file);

    time_reading = clock()-time_reading;


    //Inizialization
    for (i = 0; i < nnodes; i++) {
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
    source = BinarySearch(source_id, node_complete_variable, nnodes);
    goal = BinarySearch(goal_id, node_complete_variable, nnodes);

    printf("Calculating using %s heuristic\n", heuristics_list[heuristic_number]);

    time_A=clock();

    /* Call A_star algorithm from A_star_aux
       if no error occurs proceed to write results*/
    if (A_star(source, goal, heuristic_number) == 0) {

        time_A=clock()-time_A;
        time_total = clock() - time_total;

        node_complete *curr;
        long n;
        FILE *fp;

        /* Write results to path_file */
        if ((fp=fopen(path_file, "w")) == NULL)
            ExitError("The path_file does not exist or cannot be opened", 1);

        fprintf(fp, "# path from goal to source\n");
        fprintf(fp, "# Time elapsed by A* Algorithm %fs \n",    ((double)time_A)/CLOCKS_PER_SEC);
        fprintf(fp, "# Time elapsed reading binary file %fs\n", ((double)time_reading)/CLOCKS_PER_SEC);
        fprintf(fp, "# Total time elapsed %fs\n",               ((double)time_total)/CLOCKS_PER_SEC);
        fprintf(fp, "# Iteration, Node Id, Distance, latitude, longitude\n");

        for (n=0, curr=&(node_complete_variable[goal]); curr; n++, curr=curr->parent)
            fprintf(fp, "%4.0ld, %10.0lu, %6.2f, %2.7f, %2.7f\n", n, curr->id, curr->g, curr->lat, curr->lon);

        fclose(fp);

        printf("Time elapsed by A* Algorithm %fs\n", ((double)time_A)/CLOCKS_PER_SEC );
    }

    else {
        printf("Error, no path from source to goal\n");
        time_total = clock() - time_total;
    }

    printf("Total execution time: %fs\n",((double)time_total)/CLOCKS_PER_SEC);
    printf("Time reading binary file: %fs\n",((double)time_reading)/CLOCKS_PER_SEC);

    return 0;
}
