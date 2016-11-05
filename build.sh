#! /bin/bash

if [ -s ./excruse ]; then
    rm -f ./excruse
fi

clear
gcc excruse.c -o excruse\
 -I/usr/X11R6/include/\
 -L/usr/X11R6/lib -lc -lm -lX11 -lXi -lglut -lGL -lGLU \
 -std=gnu99 -O3 -Ofast -Wall -Wextra &&\
./excruse



