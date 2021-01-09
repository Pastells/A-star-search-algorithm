A\* algorithm to find the shortest path between two nodes from a list, given their coordinates.
Works for the csv files given in http://mat.uab.cat/~alseda/MasterOpt

Compile with ```./compile.sh```

Execution:

+ ```./create_binary``` to create the binary file

+ ```./A_star <heuristic_number>``` to run the algorithm with a chosen heuristic from list:
    0. Dijkstra
    1. Haversine (default if no argument given)
    2. Spherical law of cosines
    3. Equirectangular approximation

By default the binary file is created as data/binary.bin, the path is computed to go from node 240949599 to 195977239 and the result is stored in ```results/optimal_path.csv```
These can be changed with ```argv[2-5]```. For example a correct execution may be:

```./A_star 2 771979683 429854583 data/binary.bin results/girona_lleida.csv```
