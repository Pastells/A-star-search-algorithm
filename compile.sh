#!/bin/bash
SRC="src/"
gcc -ofast -Wall -Wextra -o create_binary ${SRC}create_binary.c -lm
gcc -ofast -Wall -Wextra -o A_star ${SRC}A_star.c ${SRC}A_star_aux.c -lm
