#!/bin/bash
SRC="src/"
gcc="gcc -ofast -Wall -Wextra -o"
${gcc} create_binary ${SRC}create_binary.c ${SRC}utils.c -lm
${gcc} A_star ${SRC}A_star.c ${SRC}A_star_aux.c ${SRC}utils.c -lm
