#include "SimulationMPI.hpp"
#define MASTER 0

namespace nemo {
	namespace mpi_dist {

SimulationMPI::SimulationMPI(const nemo::Network *net, const nemo::Configuration& conf, int argc, char* argv [], unsigned duration, bool timed)
{
	MPI::Init(argc, argv);
	unsigned rank = MPI::COMM_WORLD.Get_rank();
	unsigned workers = MPI::COMM_WORLD.Get_size();
	if (rank == MASTER) {
		nemo::mpi_dist::MasterSimulation(*net, conf, duration, timed);
	} else {
		nemo::mpi_dist::WorkerSimulation(rank, workers);
	}
	MPI::Finalize();
}

SimulationMPI::~SimulationMPI()
{
	;
}

	}
}
