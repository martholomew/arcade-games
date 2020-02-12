#!/bin/bash

gcc main.c -o bin/program -l SDL2 -l SDL2_ttf \
&& ./bin/program
