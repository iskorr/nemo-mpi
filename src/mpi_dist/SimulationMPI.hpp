#ifndef MPI_SIMULATION_HPP
#define MPI_SIMULATION_HPP
#include "MasterSimulation.hpp"
#include "WorkerSimulation.hpp"

namespace nemo {
	namespace mpi_dist {

class SimulationMPI {
public:
	SimulationMPI(const nemo::Network *net, const nemo::Configuration& conf, int argc, char* argv [], unsigned duration, bool timed, const std::string& file);
	~SimulationMPI();
};

	}
}
#endif
