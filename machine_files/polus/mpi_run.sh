module load SpectrumMPI
mpicc mpi_sor_3d.c -o test -std=c99
bsub < mpi_config
bjobs
