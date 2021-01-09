/* Call A_star algorithm with a heuristic of the heuristics_list
   and write result to file
   Pol Pastells, Narcís Font, January 2021 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "utils.h"
#include "a_star_aux.h"

/*************************************************************************/

node_dist *node_dist_var;
node *nodes;


int main (int argc, char *argv[]) {
    // Declarations
    unsigned int i;
    unsigned long nnodes = 23895681UL;
    int node_start, node_goal;
    char *heuristics_list[] = {
        "0. Dijkstra",
        "1. Haversine",
        "2. Spherical law of cosines",
        "3. Equirectangular approxmation"};

    // times
    time_t time_A;
    time_t time_reading;
    time_t time_total;
    time_total = clock();

    /* Default options, can be overwritten with arguments*/
    int heuristic_number = 1;
    int node_start_id = 240949599;
    int node_goal_id = 195977239;
    char *binary_file = "data/binary.bin";
    char *path_file= "results/optimal_path.csv";

    if (argc>1) {heuristic_number = atoi(argv[1]);}
    if (argc>2) {node_start_id        = atoi(argv[2]);}
    if (argc>3) {node_goal_id          = atoi(argv[3]);}
    if (argc>4) {binary_file      =      argv[4];}
    if (argc>5) {path_file        =      argv[5];}

    if (heuristic_number > 3) {
        ExitError("Heuristic number does not exist, use 0-3", 1);
    }

    node_dist_var = malloc ( sizeof(node_dist)*nnodes);

    // Read file
    time_reading = clock();
    Reading_csv(binary_file);
    time_reading = clock()-time_reading;

    // Initialization
    for (i = 0; i < nnodes; i++)
        NodeInit(i);
    free(nodes);

    // Index of node_start and node_goal
    node_start = BinarySearch(node_start_id, node_dist_var, nnodes);
    node_goal  = BinarySearch(node_goal_id,  node_dist_var, nnodes);

    printf("Calculating using %s heuristic\n", heuristics_list[heuristic_number]);


    /* Call a_star algorithm from a_star_aux
       if no error occurs proceed to write results*/
    time_A=clock();
    if (A_star(node_start, node_goal, heuristic_number) == 0) {
        time_A=clock()-time_A;
        time_total = clock() - time_total;

        node_dist *curr;
        long n;
        FILE *fp;

        /* Write results to path_file */
        if ((fp=fopen(path_file, "w")) == NULL)
            ExitError("The path_file does not exist or cannot be opened", 2);

        fprintf(fp, "# path from node_goal to node_start\n");
        fprintf(fp, "# Time elapsed by A* algorithm %fs \n",    ((double)time_A)/CLOCKS_PER_SEC);
        fprintf(fp, "# Time elapsed reading binary file %fs\n", ((double)time_reading)/CLOCKS_PER_SEC);
        fprintf(fp, "# Total time elapsed %fs\n",               ((double)time_total)/CLOCKS_PER_SEC);
        fprintf(fp, "# Longitude, latitude node_id, distance\n");

        /* At some point we flipped longitude and latitude so I correct it here */
        for (n=0, curr=&(node_dist_var[node_goal]); curr; n++, curr=curr->parent)
            fprintf(fp, "%2.7f, %2.7f, %10.0lu, %6.2f\n", curr->lat, curr->lon, curr->id, curr->g);

        fclose(fp);

        printf("Time elapsed by A* algorithm %fs\n", ((double)time_A)/CLOCKS_PER_SEC );
    }

    else {
        time_total = clock() - time_total;
        printf("Error, no path from node_start to node_goal\n");
    }
    printf("Total execution time: %fs\n",((double)time_total)/CLOCKS_PER_SEC);
    printf("Time reading binary file: %fs\n",((double)time_reading)/CLOCKS_PER_SEC);
    return 0;
}
