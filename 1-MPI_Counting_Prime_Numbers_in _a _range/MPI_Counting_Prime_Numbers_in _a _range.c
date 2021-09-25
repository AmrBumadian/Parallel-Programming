#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mpi.h"

int is_prime(int num) {
    if (num == 0 || num == 1) return 0; // Not Prime
    for (int i = 2; i*i <= num; i++) {
        if (num % i == 0) return 0; // Not Prime
    }
    return 1; // Prime
}

int count_primes(int start, int end) { // count the number of primes in a range
    int count = 0;
    for (int i = start; i < end; i++) {
        if (is_prime(i)) count++;
    }
    return count;
}

int main(int argc, char *argv[]) {
    int my_rank; // Rank of the process
    int p; // Number of Processes
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &p);

    if (my_rank == 0) { // Master

        int l, r; // range
        scanf("%d %d", &l, &r);
        int globalCount = 0; // total number of prime numbers
        int size = r - l;

        int portionSize = size / (p - 1); // sub-range
        int x, y;
        for (int i = 1; i < p; i++) { // send messages to the processes
            MPI_Send(&l, 1, MPI_INT, i, 0, MPI_COMM_WORLD); // sending low point
            MPI_Send(&portionSize, 1, MPI_INT, i, 1, MPI_COMM_WORLD); // sending portion size
        }

        for (int i = 1; i < p; i++) {
            int subCount;
            MPI_Recv(&subCount, 1, MPI_INT, i, 2, MPI_COMM_WORLD, &status); // receive each sub-count
            globalCount += subCount;
        }
        printf("Total number of prime numbers is: %d\n", globalCount);

    } else {
        // Portion according to rank
        int portionSize;
        int low;
        MPI_Recv(&low, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status); // receive low point
        MPI_Recv(&portionSize, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &status); // receive portions size

        // the range
        int x = low + (my_rank - 1) * portionSize; // the left value for the current portion
        int y = low + (my_rank) * portionSize; // the right value for the current portion

        int subCount = count_primes(x, y); // count the number of primes in this range
        printf("Total number of prime numbers in P%d is: %d\n", my_rank, subCount);
        MPI_Send(&subCount, 1, MPI_INT, 0, 2, MPI_COMM_WORLD); // send the sub-count to the master
    }

    MPI_Finalize(); // exit
    return 0;
}
