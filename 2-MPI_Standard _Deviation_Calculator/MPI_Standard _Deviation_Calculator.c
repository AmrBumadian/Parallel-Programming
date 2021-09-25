#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "mpi.h"

int main(int argc, char *argv[]) {

    int my_rank; // Rank of the process
    int p; // Number of Processes

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &p);

    if (my_rank == 0) { // Master
        int n;
		printf("Enter N: ");
        scanf("%d", &n);
        MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

        long long local_sum = 0, global_sum = 0;
        MPI_Allreduce(&local_sum, &global_sum, 1, MPI_LONG_LONG, MPI_SUM, MPI_COMM_WORLD);

        double number = 0;
        double total_mean_squared_diff;
        MPI_Reduce(&number, &total_mean_squared_diff, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

        double ans = sqrt(total_mean_squared_diff / (n * (p - 1)));
		printf("=================================================\n");
        printf("Standard Deviation = %lf\n", ans);
		printf("=================================================\n");

    } else {
        int n;
        MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

        srand(time(NULL));
        int local_num_set[n];
        long long local_sum = 0, global_sum = 0;
		int i;
		printf("P%d data: ", my_rank);
        for (i = 0; i < n; ++i) {
            local_num_set[i] = my_rank*(rand() % 100);
			printf("%d ",local_num_set[i]);
            local_sum += local_num_set[i];
        }
		printf("\n=================================================\n");
        MPI_Allreduce(&local_sum, &global_sum, 1, MPI_LONG_LONG, MPI_SUM, MPI_COMM_WORLD);
        double global_mean = (double) global_sum / (n * (p - 1));
        double local_mean_squared_diff;

        for (i = 0; i < n; ++i) {
            double mean_squared_diff = global_mean - local_num_set[i];
            local_mean_squared_diff += mean_squared_diff * mean_squared_diff;
        }
		
        MPI_Reduce(&local_mean_squared_diff, NULL, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize(); // exit
    return 0;
}
