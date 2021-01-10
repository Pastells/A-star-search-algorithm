# A\* algorithm to find the shortest path between two nodes from a list, given their coordinates.
Works for the csv files given in http://mat.uab.cat/~alseda/MasterOpt


## Execution:

+ Place input csv in data folder

+ Compile with ```./compile.sh```

+ ```./create_binary``` to create the binary file

+ ```./a_star <heuristic_number>``` to run the algorithm with a chosen heuristic from list:
    0. Dijkstra
    1. Haversine (default if no argument given)
    2. Spherical law of cosines
    3. Equirectangular approximation

+ Visualize results with ```python ./visualization.py``` (requisite: gmplot)

By default the binary file is created as data/binary.bin,
the path is computed to go from node 240949599 to 195977239
and the result is stored in ```results/optimal_path.csv```
These can be changed with ```argv[2-5]```. An example of a correct execution may be:

```./a_star 2 771979683 429854583 data/binary.bin results/girona_lleida.csv```

## Structure

### src

Contains source codes to be compiled with compile.sh

+ create_binary.c reads csv file and generates binary file

+ a_star.c is the main module, calling functions from a_star_aux

+ utils.c/utils.h contain shared function for a_star.c, a_star_aux.c and create_binary.c

### data

Contains input csv's and binary files

### results

Contains output csv's and html's with map
