name=$1
mpicc mpi_sor_3d.c -o $name -std=c99

mpisubmit.bg --stdout 'mpi/mpi.$(jobid).out' --stderr 'mpi/mpi.$(jobid).err' -n 1 ./$name
mpisubmit.bg --stdout 'mpi/mpi.$(jobid).out' --stderr 'mpi/mpi.$(jobid).err' -n 2 ./$name
mpisubmit.bg --stdout 'mpi/mpi.$(jobid).out' --stderr 'mpi/mpi.$(jobid).err' -n 4 ./$name
mpisubmit.bg --stdout 'mpi/mpi.$(jobid).out' --stderr 'mpi/mpi.$(jobid).err' -n 8 ./$name
mpisubmit.bg --stdout 'mpi/mpi.$(jobid).out' --stderr 'mpi/mpi.$(jobid).err' -n 16 ./$name
mpisubmit.bg --stdout 'mpi/mpi.$(jobid).out' --stderr 'mpi/mpi.$(jobid).err' -n 32 ./$name
mpisubmit.bg --stdout 'mpi/mpi.$(jobid).out' --stderr 'mpi/mpi.$(jobid).err' -n 64 ./$name
mpisubmit.bg --stdout 'mpi/mpi.$(jobid).out' --stderr 'mpi/mpi.$(jobid).err' -n 128 ./$name
mpisubmit.bg --stdout 'mpi/mpi.$(jobid).out' --stderr 'mpi/mpi.$(jobid).err' -n 256 -w 600s ./$name
mpisubmit.bg --stdout 'mpi/mpi.$(jobid).out' --stderr 'mpi/mpi.$(jobid).err' -n 512 -w 300s ./$name

