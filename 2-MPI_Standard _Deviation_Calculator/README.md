1- Compile: mpicc -Wall -g -o run.out <filename> -lm
-lm : link the libraries needed (Math.h)

2- Run : mpiexec -n <number of processes> ./run.out
3- Enter N