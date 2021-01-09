#!/bin/bash
SRC="src/"
gcc="gcc -ofast -Wall -Wextra -o"
${gcc} create_binary ${SRC}create_binary.c ${SRC}utils.c -lm
${gcc} a_star ${SRC}a_star.c ${SRC}a_star_aux.c ${SRC}utils.c -lm
