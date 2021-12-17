#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "omp.h"

#define  Max(a, b) ((a)>(b)?(a):(b))
#define  N   ((2<<5)+2)

int slider = 0;
double maxeps = 0.1e-7;
double A[2][N][N][N];

void init();

void verify();

int main(int an, char **as) {
    double time_begin = omp_get_wtime();

    int itmax = 100;


    init();

    for (int it = 1; it <= itmax; it++) {

        double eps;
        {
            eps = 0.;
        }
        //relax;
        int i, j, k;
#pragma omp parallel for private(i, j, k) shared(A, slider) reduction(max:eps)
        for (i = 1; i <= N - 2; i++) {
            for (j = 1; j <= N - 2; j++) {
                for (k = 1; k <= N - 2; k++) {
                    double e = A[slider][i][j][k];
                    A[slider ^ 1][i][j][k] = (A[slider][i - 1][j][k] + A[slider][i + 1][j][k]
                                              + A[slider][i][j - 1][k] + A[slider][i][j + 1][k]
                                              + A[slider][i][j][k - 1] + A[slider][i][j][k + 1]) / 6.;
                    eps = Max(eps, fabs(e - A[slider ^ 1][i][j][k]));
                }
            }
        }
        slider ^= 1;




//        printf("it=%4i   eps=%f\n", it, eps);
        printf("it=%4i slider = %3d ", it, slider);
        verify();
        if (eps < maxeps) {
            break;
        }
    }

    verify();

//    fprintf(stdout, "time = %lf\n", omp_get_wtime() - time_begin);
    printf("%10d %5d ",N, omp_get_max_threads());
    printf("%10.5lf\n", omp_get_wtime() - time_begin);
    return 0;
}

void init() {
    int i, j, k;
#pragma omp parallel for private(i, j, k) shared(A, slider)
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
#pragma omp parallel for private(i, j, k) shared(A, slider) reduction(+:s)
    for (i = 0; i <= N - 1; i++) {
        for (j = 0; j <= N - 1; j++) {
            for (k = 0; k <= N - 1; k++) {
                s = s + A[slider][i][j][k] * (i + 1) * (j + 1) * (k + 1) / (N * N * N);
            }
        }
    }
    printf("  S cur  = %.3f\n", s);
//    printf("  S true = 2814659.450575\n");
//    if (s - 2814659.450575 < maxeps) {
//Size Thread num time
//        printf("Thread num  = %d\n", omp_get_max_threads());
//        printf("Size  = %d\n", N);
//    }
}