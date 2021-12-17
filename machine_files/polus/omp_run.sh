xlc -qsmp=omp omp_sor_3d.c -o test
bsub < omp_config
bjobs
