./random output_runtime/rand.txt -n 2000 -m 1000
./random output_runtime/rand.txt -n 2000 -m 1000
./random output_runtime/rand.txt -n 2000 -m 1000
./random output_runtime/rand.txt -n 2000 -m 1000
./random output_runtime/rand.txt -n 2000 -m 1000
./random output_runtime/rand.txt -n 2000 -m 1000
./random output_runtime/rand.txt -n 2000 -m 1000

mpirun -np 2 ./mpi_dist_general 2000 1000 0 100 1 0 output_runtime/mpi1.txt
mpirun -np 3 ./mpi_dist_general 2000 1000 0 100 1 0 output_runtime/mpi2.txt
mpirun -np 4 ./mpi_dist_general 2000 1000 0 100 1 0 output_runtime/mpi3.txt
mpirun -np 5 ./mpi_dist_general 2000 1000 0 100 1 0 output_runtime/mpi4.txt
mpirun -np 6 ./mpi_dist_general 2000 1000 0 100 1 0 output_runtime/mpi5.txt

mpirun -np 2 ./mpi_dist_general 2000 1000 0 200 1 0 output_runtime/mpi1.txt
mpirun -np 3 ./mpi_dist_general 2000 1000 0 200 1 0 output_runtime/mpi2.txt
mpirun -np 4 ./mpi_dist_general 2000 1000 0 200 1 0 output_runtime/mpi3.txt
mpirun -np 5 ./mpi_dist_general 2000 1000 0 200 1 0 output_runtime/mpi4.txt
mpirun -np 6 ./mpi_dist_general 2000 1000 0 200 1 0 output_runtime/mpi5.txt

mpirun -np 2 ./mpi_dist_general 2000 1000 0 500 1 0 output_runtime/mpi1.txt
mpirun -np 3 ./mpi_dist_general 2000 1000 0 500 1 0 output_runtime/mpi2.txt
mpirun -np 4 ./mpi_dist_general 2000 1000 0 500 1 0 output_runtime/mpi3.txt
mpirun -np 5 ./mpi_dist_general 2000 1000 0 500 1 0 output_runtime/mpi4.txt
mpirun -np 6 ./mpi_dist_general 2000 1000 0 500 1 0 output_runtime/mpi5.txt

mpirun -np 2 ./mpi_dist_general 2000 1000 0 1000 1 0 output_runtime/mpi1.txt
mpirun -np 3 ./mpi_dist_general 2000 1000 0 1000 1 0 output_runtime/mpi2.txt
mpirun -np 4 ./mpi_dist_general 2000 1000 0 1000 1 0 output_runtime/mpi3.txt
mpirun -np 5 ./mpi_dist_general 2000 1000 0 1000 1 0 output_runtime/mpi4.txt
mpirun -np 6 ./mpi_dist_general 2000 1000 0 1000 1 0 output_runtime/mpi5.txt

mpirun -np 2 ./mpi_dist_general 2000 1000 0 2000 1 0 output_runtime/mpi1.txt
mpirun -np 3 ./mpi_dist_general 2000 1000 0 2000 1 0 output_runtime/mpi2.txt
mpirun -np 4 ./mpi_dist_general 2000 1000 0 2000 1 0 output_runtime/mpi3.txt
mpirun -np 5 ./mpi_dist_general 2000 1000 0 2000 1 0 output_runtime/mpi4.txt
mpirun -np 6 ./mpi_dist_general 2000 1000 0 2000 1 0 output_runtime/mpi5.txt

mpirun -np 2 ./mpi_dist_general 2000 1000 0 5000 1 0 output_runtime/mpi1.txt
mpirun -np 3 ./mpi_dist_general 2000 1000 0 5000 1 0 output_runtime/mpi2.txt
mpirun -np 4 ./mpi_dist_general 2000 1000 0 5000 1 0 output_runtime/mpi3.txt
mpirun -np 5 ./mpi_dist_general 2000 1000 0 5000 1 0 output_runtime/mpi4.txt
mpirun -np 6 ./mpi_dist_general 2000 1000 0 5000 1 0 output_runtime/mpi5.txt

mpirun -np 2 ./mpi_dist_general 2000 1000 0 10000 1 0 output_runtime/mpi1.txt
mpirun -np 3 ./mpi_dist_general 2000 1000 0 10000 1 0 output_runtime/mpi2.txt
mpirun -np 4 ./mpi_dist_general 2000 1000 0 10000 1 0 output_runtime/mpi3.txt
mpirun -np 5 ./mpi_dist_general 2000 1000 0 10000 1 0 output_runtime/mpi4.txt
mpirun -np 6 ./mpi_dist_general 2000 1000 0 10000 1 0 output_runtime/mpi5.txt
