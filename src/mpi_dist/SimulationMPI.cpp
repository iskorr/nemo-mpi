#include "SimulationMPI.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MASTER 0
using namespace std;

namespace nemo {
	namespace mpi_dist {

SimulationMPI::SimulationMPI(const nemo::Network *net, const nemo::Configuration& conf, int argc, char* argv [], unsigned duration, bool timed, const string& filename) : neurons(100), synapses(50)
{
	MPI::Init(argc, argv);
	if (argc > 1) neurons = atoi(argv[1]);
	if (argc > 2) synapses = atoi(argv[2]);
	unsigned rank = MPI::COMM_WORLD.Get_rank();
	unsigned workers = MPI::COMM_WORLD.Get_size();
	if (rank == MASTER) {
		ofstream file;
		file.open(filename.c_str(), fstream::app);
		ostream& out = filename.empty() ? cout : file;
		nemo::mpi_dist::MasterSimulation sim(*net, conf);
		if (timed) simulateTimed(sim, duration, out);
		else simulateStepped(sim, duration, out);
		file.close();
	} else {
		nemo::mpi_dist::WorkerSimulation(rank, workers);
	}
	MPI::Finalize();
}

SimulationMPI::~SimulationMPI()
{
	;
}

void
SimulationMPI::simulateTimed(nemo::mpi_dist::MasterSimulation& master, unsigned duration, ostream& out)
{
	unsigned totalfiring = 0;
	nemo::Timer timer;
	timer.reset();
	while(timer.elapsedWallclock() < duration) {
		totalfiring += master.step();
		timer.step();
	}
	unsigned long runtime = timer.elapsedWallclock();
	unsigned totalspiking = master.endSimulation();
	out << neurons << " " << synapses << " " << runtime << " " << timer.elapsedSimulation() << " " << totalfiring << " " << totalspiking << "\n";
	cout << "Total runtime of the simulation: " << runtime << "\n";
	cout << "Total # of steps: " << timer.elapsedSimulation() << "\n";
	cout << "Total # of neurons fired: " << totalfiring << "\n";
	cout << "Total # of spikes delivered (Over MPI): " << totalspiking << "\n";
}

void
SimulationMPI::simulateStepped(nemo::mpi_dist::MasterSimulation& master, unsigned duration, ostream& out)
{
	unsigned totalfiring = 0;
	nemo::Timer timer;
	timer.reset();
	while(timer.elapsedSimulation() < duration) {
		totalfiring += master.step();
		timer.step();
	}
	unsigned long runtime = timer.elapsedWallclock();
	unsigned totalspiking = master.endSimulation();
	out << neurons << " " << synapses << " " << runtime << " " << timer.elapsedSimulation() << " " << totalfiring << " " << totalspiking << "\n";
	cout << "Total runtime of the simulation: " << runtime << "\n";
	cout << "Total # of steps: " << timer.elapsedSimulation() << "\n";
	cout << "Total # of neurons fired: " << totalfiring << "\n";
	cout << "Total # of spikes delivered (Over MPI): " << totalspiking << "\n";
}

	}
}
