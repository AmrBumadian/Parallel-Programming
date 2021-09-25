#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <omp.h>


void solve(char ** matrix, int rows, int columns, int work_per_thread);

int main(int argc, char* argv[]) {

    int thread_count = strtol(argv[1], NULL, 10);
    int rows,columns;
    printf("Enter number of rows and columns: ");
    scanf("%d %d", &rows, &columns);

    char **matrix = (char **)malloc(rows * sizeof(char *));
    int i;
    for (i=0; i<rows; i++) {
        matrix[i] = (char *)malloc(columns * sizeof(char));
    }

    bool finished = false;
    while (!finished) {
        finished = true;
        bool interrupted = false;
        printf("Enter the matrix row-wise:\n");
        for (i=0;i<rows;++i) {
            int j;
            for (j=0;j<columns;++j) {
                char c;
                scanf(" %c", &c);
                if (c!= 'X' && c!= 'O') {
                    printf("***Invalid Matrix Element***\n\n");
                    interrupted = true;
                    break;
                }
                matrix[i][j] = c;
            }
            if (interrupted) {
                finished = false;
                break;
            }
        }
    }
    
	/* Alternate Solution
	 * pragma omp parallel num_threads(thread_count)
	 * solve(matrix, rows, columns, ceil((double)rows/thread_count));
	 */

    solve_for(matrix, rows, columns, thread_count);

	printf("\n");
	for (i=0;i<rows;++i) {
        int j;
        for (j=0;j<columns;++j) {
           printf("%c ", matrix[i][j]);
        }
        printf("\n");
    }
    return 0;
}

void solve(char **matrix, int rows, int columns, int work_per_thread) {
	int thread_number = omp_get_thread_num();
    int n =  thread_number * work_per_thread;
    while (n < rows && work_per_thread--) {
        int j;
        for (j=0;j<columns;++j) {
            bool found = false;
            if (matrix[n][j] == 'O') {
                found |= (j-1 >= 0) ? matrix[n][j-1] == 'X': false;
                found &= (j+1 < columns) ? matrix[n][j+1] == 'X': false;
            }
            if (found){ matrix[n][j] = 'X'; printf("Thread no.%d changed Matrix[%d][%d]\n", thread_number,n,j);}
        }
        ++n;
    }
}

void solve_for(char **matrix, int rows, int columns, int thread_count) {
    
    #pragma omp parallel for num_threads(thread_count) schedule(static, 1);
	for (int i=0;i<rows;++i) {
        for (int j=0;j<columns;++j) {
            bool found = false;
            if (matrix[i][j] == 'O') {
                found |= (j-1 >= 0) ? matrix[i][j-1] == 'X': false;
                found &= (j+1 < columns) ? matrix[i][j+1] == 'X': false;
            }
            if (found){ 
                matrix[i][j] = 'X'; 
                printf("Thread no.%d changed Matrix[%d][%d]\n", omp_get_thread_num(),i,j);
            }
        }
    }
}
