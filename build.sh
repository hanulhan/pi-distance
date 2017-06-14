#!/bin/bash
gcc  -o distance distance.c -I /usr/local/include -L /usr/local/lib -lpigpio -lrt -lpthread
