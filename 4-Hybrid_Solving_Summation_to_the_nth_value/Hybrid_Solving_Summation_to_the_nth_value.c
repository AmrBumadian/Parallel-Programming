#include <stdio.h>
#include <mpi.h>
#include <omp.h>

/* fast power function */
long long power(long long x, int y);

/* distribute the process portion to a team of threads*/
long double summation(long long start, long long end, int number_of_threads);

int main(int argc, char* argv[]) {
    
    int process_rank;
    int number_of_processes;

    MPI_Init(&argc, &argv); // initialize the processes
    MPI_Comm_rank(MPI_COMM_WORLD, &process_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &number_of_processes);
    
    
    long long process_portion;
    long double process_sum = 0.0;
    int number_of_threads;
    
    int N;
    long long start, end;
    
    if (process_rank == 0) {
        printf("Enter N (The index of summation): ");
        scanf("%d",&N);
        printf("Enter Number of Threads/Process: ");
        scanf("%d",&number_of_threads);
        process_portion = N/number_of_processes;
    }

    /* broadcasting the variables from process 0 to other processes */
    MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&process_portion, 1, MPI_LONG_LONG, 0, MPI_COMM_WORLD);
    MPI_Bcast(&number_of_threads, 1, MPI_INT, 0, MPI_COMM_WORLD);

    /* calculate the start and the end of the process portion */
    start = process_rank*process_portion + 1;
    end = start + process_portion + ((process_rank == number_of_processes-1) ? N%number_of_processes: 0);
    process_sum += summation(start, end, number_of_threads);
    
    if (process_rank == 0) {
        long double total_sum = 0.0;
        /* sum the result of all the processes */
        MPI_Reduce(&process_sum, &total_sum, 1, MPI_LONG_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
        printf("Total Sum is: %.40Lf\n", total_sum);
        printf("Error is: %.40Lf\n", 1.0L-total_sum);
    } else {
        MPI_Reduce(&process_sum, NULL, 1, MPI_LONG_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    }
    MPI_Finalize(); // exit
}

long long power(long long x, int y) { 

    long long ans;
    if(!y) return 1;
    ans = power(x, y / 2);
    if (y&1) {
        if(y > 0) return x * ans * ans;
        else return (ans * ans) / x;
    } else return ans * ans;

}

long double summation(long long start, long long end, int number_of_threads) { 

    long double local_sum = 0.0;
	long long i;

    /* distribute the loop iterations over the team of threads in a round-robin fashion 1 at a time */
    #pragma omp parallel for num_threads(number_of_threads) \
            reduction(+: local_sum) schedule(static, 1)
            for (i=start; i < end; ++i) {
                local_sum += 1.0L/power(2,i);
            }
    return local_sum;
}
