./random output_synapse/rand.txt -n 2000 -m 100
./random output_synapse/rand.txt -n 2000 -m 200
./random output_synapse/rand.txt -n 2000 -m 500
./random output_synapse/rand.txt -n 2000 -m 1000
./random output_synapse/rand.txt -n 2000 -m 2000
./random output_synapse/rand.txt -n 2000 -m 5000

mpirun -np 2 ./mpi_dist_general 2000 100 0 500 1 0 output_synapse/mpi1.txt
mpirun -np 3 ./mpi_dist_general 2000 100 0 500 1 0 output_synapse/mpi2.txt
mpirun -np 4 ./mpi_dist_general 2000 100 0 500 1 0 output_synapse/mpi3.txt
mpirun -np 5 ./mpi_dist_general 2000 100 0 500 1 0 output_synapse/mpi4.txt
mpirun -np 6 ./mpi_dist_general 2000 100 0 500 1 0 output_synapse/mpi5.txt

mpirun -np 2 ./mpi_dist_general 2000 200 0 500 1 0 output_synapse/mpi1.txt
mpirun -np 3 ./mpi_dist_general 2000 200 0 500 1 0 output_synapse/mpi2.txt
mpirun -np 4 ./mpi_dist_general 2000 200 0 500 1 0 output_synapse/mpi3.txt
mpirun -np 5 ./mpi_dist_general 2000 200 0 500 1 0 output_synapse/mpi4.txt
mpirun -np 6 ./mpi_dist_general 2000 200 0 500 1 0 output_synapse/mpi5.txt

mpirun -np 2 ./mpi_dist_general 2000 500 0 500 1 0 output_synapse/mpi1.txt
mpirun -np 3 ./mpi_dist_general 2000 500 0 500 1 0 output_synapse/mpi2.txt
mpirun -np 4 ./mpi_dist_general 2000 500 0 500 1 0 output_synapse/mpi3.txt
mpirun -np 5 ./mpi_dist_general 2000 500 0 500 1 0 output_synapse/mpi4.txt
mpirun -np 6 ./mpi_dist_general 2000 500 0 500 1 0 output_synapse/mpi5.txt

mpirun -np 2 ./mpi_dist_general 2000 1000 0 500 1 0 output_synapse/mpi1.txt
mpirun -np 3 ./mpi_dist_general 2000 1000 0 500 1 0 output_synapse/mpi2.txt
mpirun -np 4 ./mpi_dist_general 2000 1000 0 500 1 0 output_synapse/mpi3.txt
mpirun -np 5 ./mpi_dist_general 2000 1000 0 500 1 0 output_synapse/mpi4.txt
mpirun -np 6 ./mpi_dist_general 2000 1000 0 500 1 0 output_synapse/mpi5.txt

mpirun -np 2 ./mpi_dist_general 2000 2000 0 500 1 0 output_synapse/mpi1.txt
mpirun -np 3 ./mpi_dist_general 2000 2000 0 500 1 0 output_synapse/mpi2.txt
mpirun -np 4 ./mpi_dist_general 2000 2000 0 500 1 0 output_synapse/mpi3.txt
mpirun -np 5 ./mpi_dist_general 2000 2000 0 500 1 0 output_synapse/mpi4.txt
mpirun -np 6 ./mpi_dist_general 2000 2000 0 500 1 0 output_synapse/mpi5.txt

mpirun -np 2 ./mpi_dist_general 2000 5000 0 500 1 0 output_synapse/mpi1.txt
mpirun -np 3 ./mpi_dist_general 2000 5000 0 500 1 0 output_synapse/mpi2.txt
mpirun -np 4 ./mpi_dist_general 2000 5000 0 500 1 0 output_synapse/mpi3.txt
mpirun -np 5 ./mpi_dist_general 2000 5000 0 500 1 0 output_synapse/mpi4.txt
mpirun -np 6 ./mpi_dist_general 2000 5000 0 500 1 0 output_synapse/mpi5.txt
