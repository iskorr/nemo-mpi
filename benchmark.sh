./random output/rand.txt -n 1000 -m 300
./random output/rand.txt -n 2000 -m 300
./random output/rand.txt -n 5000 -m 300
./random output/rand.txt -n 10000 -m 300
./random output/rand.txt -n 20000 -m 300
./random output/rand.txt -n 50000 -m 300
./random output/rand.txt -n 100000 -m 300
mpirun -np 2 ./mpi_dist_general 1000 300 0 500 1 0 output/mpi1.txt
mpirun -np 3 ./mpi_dist_general 1000 300 0 500 1 0 output/mpi2.txt
mpirun -np 4 ./mpi_dist_general 1000 300 0 500 1 0 output/mpi3.txt
mpirun -np 5 ./mpi_dist_general 1000 300 0 500 1 0 output/mpi4.txt
mpirun -np 6 ./mpi_dist_general 1000 300 0 500 1 0 output/mpi5.txt
mpirun -np 2 ./mpi_dist_general 2000 300 0 500 1 0 output/mpi1.txt
mpirun -np 3 ./mpi_dist_general 2000 300 0 500 1 0 output/mpi2.txt
mpirun -np 4 ./mpi_dist_general 2000 300 0 500 1 0 output/mpi3.txt
mpirun -np 5 ./mpi_dist_general 2000 300 0 500 1 0 output/mpi4.txt
mpirun -np 6 ./mpi_dist_general 2000 300 0 500 1 0 output/mpi5.txt
mpirun -np 2 ./mpi_dist_general 5000 300 0 500 1 0 output/mpi1.txt
mpirun -np 3 ./mpi_dist_general 5000 300 0 500 1 0 output/mpi2.txt
mpirun -np 4 ./mpi_dist_general 5000 300 0 500 1 0 output/mpi3.txt
mpirun -np 5 ./mpi_dist_general 5000 300 0 500 1 0 output/mpi4.txt
mpirun -np 6 ./mpi_dist_general 5000 300 0 500 1 0 output/mpi5.txt
mpirun -np 2 ./mpi_dist_general 10000 300 0 500 1 0 output/mpi1.txt
mpirun -np 3 ./mpi_dist_general 10000 300 0 500 1 0 output/mpi2.txt
mpirun -np 4 ./mpi_dist_general 10000 300 0 500 1 0 output/mpi3.txt
mpirun -np 5 ./mpi_dist_general 10000 300 0 500 1 0 output/mpi4.txt
mpirun -np 6 ./mpi_dist_general 10000 300 0 500 1 0 output/mpi5.txt
mpirun -np 2 ./mpi_dist_general 20000 300 0 500 1 0 output/mpi1.txt
mpirun -np 3 ./mpi_dist_general 20000 300 0 500 1 0 output/mpi2.txt
mpirun -np 4 ./mpi_dist_general 20000 300 0 500 1 0 output/mpi3.txt
mpirun -np 5 ./mpi_dist_general 20000 300 0 500 1 0 output/mpi4.txt
mpirun -np 6 ./mpi_dist_general 20000 300 0 500 1 0 output/mpi5.txt
mpirun -np 2 ./mpi_dist_general 50000 300 0 500 1 0 output/mpi1.txt
mpirun -np 3 ./mpi_dist_general 50000 300 0 500 1 0 output/mpi2.txt
mpirun -np 4 ./mpi_dist_general 50000 300 0 500 1 0 output/mpi3.txt
mpirun -np 5 ./mpi_dist_general 50000 300 0 500 1 0 output/mpi4.txt
mpirun -np 6 ./mpi_dist_general 50000 300 0 500 1 0 output/mpi5.txt
mpirun -np 2 ./mpi_dist_general 100000 300 0 500 1 0 output/mpi1.txt
mpirun -np 3 ./mpi_dist_general 100000 300 0 500 1 0 output/mpi2.txt
mpirun -np 4 ./mpi_dist_general 100000 300 0 500 1 0 output/mpi3.txt
mpirun -np 5 ./mpi_dist_general 100000 300 0 500 1 0 output/mpi4.txt
mpirun -np 6 ./mpi_dist_general 100000 300 0 500 1 0 output/mpi5.txt
