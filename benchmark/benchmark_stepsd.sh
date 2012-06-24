./random output_steps/rand.txt 500 -n 1000 -m 500
./random output_steps/rand.txt 1000 -n 1000 -m 500
./random output_steps/rand.txt 2000 -n 1000 -m 500
./random output_steps/rand.txt 3000 -n 1000 -m 500
./random output_steps/rand.txt 5000 -n 1000 -m 500
./random output_steps/rand.txt 10000 -n 1000 -m 500

mpirun -np 2 -machinefile mac  ./mpi_dist_general 1000 500 0 500 1 0 output_steps/mpi1.txt
mpirun -np 3 -machinefile mac  ./mpi_dist_general 1000 500 0 500 1 0 output_steps/mpi2.txt
mpirun -np 4 -machinefile mac  ./mpi_dist_general 1000 500 0 500 1 0 output_steps/mpi3.txt
mpirun -np 5 -machinefile mac  ./mpi_dist_general 1000 500 0 500 1 0 output_steps/mpi4.txt
mpirun -np 6 -machinefile mac  ./mpi_dist_general 1000 500 0 500 1 0 output_steps/mpi5.txt

mpirun -np 2 -machinefile mac  ./mpi_dist_general 1000 500 0 1000 1 0 output_steps/mpi1.txt
mpirun -np 3 -machinefile mac  ./mpi_dist_general 1000 500 0 1000 1 0 output_steps/mpi2.txt
mpirun -np 4 -machinefile mac  ./mpi_dist_general 1000 500 0 1000 1 0 output_steps/mpi3.txt
mpirun -np 5 -machinefile mac  ./mpi_dist_general 1000 500 0 1000 1 0 output_steps/mpi4.txt
mpirun -np 6 -machinefile mac  ./mpi_dist_general 1000 500 0 1000 1 0 output_steps/mpi5.txt

mpirun -np 2 -machinefile mac  ./mpi_dist_general 1000 500 0 2000 1 0 output_steps/mpi1.txt
mpirun -np 3 -machinefile mac  ./mpi_dist_general 1000 500 0 2000 1 0 output_steps/mpi2.txt
mpirun -np 4 -machinefile mac  ./mpi_dist_general 1000 500 0 2000 1 0 output_steps/mpi3.txt
mpirun -np 5 -machinefile mac  ./mpi_dist_general 1000 500 0 2000 1 0 output_steps/mpi4.txt
mpirun -np 6 -machinefile mac  ./mpi_dist_general 1000 500 0 2000 1 0 output_steps/mpi5.txt

mpirun -np 2 -machinefile mac  ./mpi_dist_general 1000 500 0 3000 1 0 output_steps/mpi1.txt
mpirun -np 3 -machinefile mac  ./mpi_dist_general 1000 500 0 3000 1 0 output_steps/mpi2.txt
mpirun -np 4 -machinefile mac  ./mpi_dist_general 1000 500 0 3000 1 0 output_steps/mpi3.txt
mpirun -np 5 -machinefile mac  ./mpi_dist_general 1000 500 0 3000 1 0 output_steps/mpi4.txt
mpirun -np 6 -machinefile mac  ./mpi_dist_general 1000 500 0 3000 1 0 output_steps/mpi5.txt

mpirun -np 2 -machinefile mac  ./mpi_dist_general 1000 500 0 5000 1 0 output_steps/mpi1.txt
mpirun -np 3 -machinefile mac  ./mpi_dist_general 1000 500 0 5000 1 0 output_steps/mpi2.txt
mpirun -np 4 -machinefile mac  ./mpi_dist_general 1000 500 0 5000 1 0 output_steps/mpi3.txt
mpirun -np 5 -machinefile mac  ./mpi_dist_general 1000 500 0 5000 1 0 output_steps/mpi4.txt
mpirun -np 6 -machinefile mac  ./mpi_dist_general 1000 500 0 5000 1 0 output_steps/mpi5.txt

mpirun -np 2 -machinefile mac  ./mpi_dist_general 1000 500 0 10000 1 0 output_steps/mpi1.txt
mpirun -np 3 -machinefile mac  ./mpi_dist_general 1000 500 0 10000 1 0 output_steps/mpi2.txt
mpirun -np 4 -machinefile mac  ./mpi_dist_general 1000 500 0 10000 1 0 output_steps/mpi3.txt
mpirun -np 5 -machinefile mac  ./mpi_dist_general 1000 500 0 10000 1 0 output_steps/mpi4.txt
mpirun -np 6 -machinefile mac  ./mpi_dist_general 1000 500 0 10000 1 0 output_steps/mpi5.txt
