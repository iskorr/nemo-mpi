./random output_neuron/rand.txt -n 1000 -m 500
./random output_neuron/rand.txt -n 2000 -m 500
./random output_neuron/rand.txt -n 5000 -m 500
./random output_neuron/rand.txt -n 10000 -m 500
./random output_neuron/rand.txt -n 20000 -m 500

mpirun -np 2 -machinefile machinefile ./mpi_dist_general 1000 500 0 500 1 0 output_neuron/mpi1.txt
mpirun -np 3 -machinefile machinefile ./mpi_dist_general 1000 500 0 500 1 0 output_neuron/mpi2.txt
mpirun -np 4 -machinefile machinefile ./mpi_dist_general 1000 500 0 500 1 0 output_neuron/mpi3.txt
mpirun -np 5 -machinefile machinefile ./mpi_dist_general 1000 500 0 500 1 0 output_neuron/mpi4.txt
mpirun -np 6 -machinefile machinefile ./mpi_dist_general 1000 500 0 500 1 0 output_neuron/mpi5.txt

mpirun -np 2 -machinefile machinefile ./mpi_dist_general 2000 500 0 500 1 0 output_neuron/mpi1.txt
mpirun -np 3 -machinefile machinefile ./mpi_dist_general 2000 500 0 500 1 0 output_neuron/mpi2.txt
mpirun -np 4 -machinefile machinefile ./mpi_dist_general 2000 500 0 500 1 0 output_neuron/mpi3.txt
mpirun -np 5 -machinefile machinefile ./mpi_dist_general 2000 500 0 500 1 0 output_neuron/mpi4.txt
mpirun -np 6 -machinefile machinefile ./mpi_dist_general 2000 500 0 500 1 0 output_neuron/mpi5.txt

mpirun -np 2 -machinefile machinefile ./mpi_dist_general 5000 500 0 500 1 0 output_neuron/mpi1.txt
mpirun -np 3 -machinefile machinefile ./mpi_dist_general 5000 500 0 500 1 0 output_neuron/mpi2.txt
mpirun -np 4 -machinefile machinefile ./mpi_dist_general 5000 500 0 500 1 0 output_neuron/mpi3.txt
mpirun -np 5 -machinefile machinefile ./mpi_dist_general 5000 500 0 500 1 0 output_neuron/mpi4.txt
mpirun -np 6 -machinefile machinefile ./mpi_dist_general 5000 500 0 500 1 0 output_neuron/mpi5.txt

mpirun -np 2 -machinefile machinefile ./mpi_dist_general 10000 500 0 500 1 0 output_neuron/mpi1.txt
mpirun -np 3 -machinefile machinefile ./mpi_dist_general 10000 500 0 500 1 0 output_neuron/mpi2.txt
mpirun -np 4 -machinefile machinefile ./mpi_dist_general 10000 500 0 500 1 0 output_neuron/mpi3.txt
mpirun -np 5 -machinefile machinefile ./mpi_dist_general 10000 500 0 500 1 0 output_neuron/mpi4.txt
mpirun -np 6 -machinefile machinefile ./mpi_dist_general 10000 500 0 500 1 0 output_neuron/mpi5.txt

mpirun -np 2 -machinefile machinefile ./mpi_dist_general 20000 500 0 500 1 0 output_neuron/mpi1.txt
mpirun -np 3 -machinefile machinefile ./mpi_dist_general 20000 500 0 500 1 0 output_neuron/mpi2.txt
mpirun -np 4 -machinefile machinefile ./mpi_dist_general 20000 500 0 500 1 0 output_neuron/mpi3.txt
mpirun -np 5 -machinefile machinefile ./mpi_dist_general 20000 500 0 500 1 0 output_neuron/mpi4.txt
mpirun -np 6 -machinefile machinefile ./mpi_dist_general 20000 500 0 500 1 0 output_neuron/mpi5.txt
