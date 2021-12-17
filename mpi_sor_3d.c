#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "mpi.h"

#define  Max(a, b) ((a)>(b)?(a):(b))
#define  N   ((2<<7)+2)

int slider = 0;
double maxeps = 0.1e-7;
double A[2][N][N][N];

void init();

void verify();

int main(int argc, char **argv) {
    int proc_num, proc_count;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &proc_num);
    MPI_Comm_size(MPI_COMM_WORLD, &proc_count);

    double time_start = MPI_Wtime();

    int line_start[proc_count];
    int line_count[proc_count];

    int n2 = N * N;

    for (int i = 0; i < proc_count; i++) {
        line_count[i] = n2 / proc_count;
    }

    for (int i = 0; i < n2 - (n2 / proc_count) * proc_count; i++) {
        line_count[i]++;
    }

    line_start[0] = 0;
    for (int i = 1; i < proc_count; i++) {
        line_start[i] = line_start[i - 1] + line_count[i - 1];
    }

    if (proc_num == -1) { // just debug information
        for (int i = 0; i < proc_count; i++) {
            printf("%5d", line_start[i]);
        }
        printf("\n");

        for (int i = 0; i < proc_count; i++) {
            printf("%5d", line_count[i]);
        }
        printf("\n");
    }

    int receive_count[proc_count];
    int start_offset[proc_count];
    for (int i = 0; i < proc_count; i++) {
        receive_count[i] = line_count[i] * N;
        start_offset[i] = line_start[i] * N;
    }

    if (proc_num == 0) { //init only in main process
        init();
    }

    int itmax = 100;
    for (int it = 1; it <= itmax; it++) {
        MPI_Bcast(A[slider], N * N * N, MPI_DOUBLE, 0, MPI_COMM_WORLD);

        double eps = 0.;
        //relax;
        int k;
        for (int iterat = line_start[proc_num]; iterat < line_start[proc_num] + line_count[proc_num]; iterat++) {
            int i_cof = iterat / N;
            int j_cof = iterat % N;
            if (i_cof >= 1 && i_cof <= (N - 2) && j_cof >= 1 && j_cof <= (N - 2)) {
                for (k = 1; k <= N - 2; k++) {
                    double e = A[slider][i_cof][j_cof][k];
                    A[slider ^ 1][i_cof][j_cof][k] = (A[slider][i_cof - 1][j_cof][k] + A[slider][i_cof + 1][j_cof][k]
                                                      + A[slider][i_cof][j_cof - 1][k] + A[slider][i_cof][j_cof + 1][k]
                                                      + A[slider][i_cof][j_cof][k - 1] +
                                                      A[slider][i_cof][j_cof][k + 1]) / 6.;
                    eps = Max(eps, fabs(e - A[slider ^ 1][i_cof][j_cof][k]));
                }
            }
        }

        slider ^= 1;

        MPI_Gatherv(A[slider][line_start[proc_num] / N][line_start[proc_num] % N], line_count[proc_num] * N, MPI_DOUBLE,
                    A[slider ^ 1][line_start[proc_num] / N][line_start[proc_num] % N], receive_count, start_offset,
                    MPI_DOUBLE, 0, MPI_COMM_WORLD);

        slider ^= 1;

        if (proc_num == -1) {  // just debug information
            printf("it=%4i slider = %3d ", it, slider);
            verify();
        }
    }

    if (proc_num == 0) {
        verify();

        //Size Thread num time
        printf("%10d %5d %10.5lf\n", N, proc_count, MPI_Wtime() - time_start);
    }


    MPI_Finalize();
    return 0;
}

void init() {
    int i, j, k;
    for (i = 0; i <= N - 1; i++) {
        for (j = 0; j <= N - 1; j++) {
            for (k = 0; k <= N - 1; k++) {
                if (i == 0 || i == N - 1 || j == 0 || j == N - 1 || k == 0 || k == N - 1) {
                    A[slider][i][j][k] = 0.;
                } else {
                    A[slider][i][j][k] = (4. + i + j + k);
                }
            }
        }
    }
}

void verify() {
    double s = 0.;
    int i, j, k;
    for (i = 0; i <= N - 1; i++) {
        for (j = 0; j <= N - 1; j++) {
            for (k = 0; k <= N - 1; k++) {
                s = s + A[slider][i][j][k] * (i + 1) * (j + 1) * (k + 1) / (N * N * N);
            }
        }
    }
}
