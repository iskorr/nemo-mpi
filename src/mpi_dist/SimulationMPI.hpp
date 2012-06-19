#ifndef MPI_SIMULATION_HPP
#define MPI_SIMULATION_HPP
#include "MasterSimulation.hpp"
#include "WorkerSimulation.hpp"
#include <nemo/Timer.hpp>

namespace nemo {
	namespace mpi_dist {

class SimulationMPI {
public:
	SimulationMPI(const nemo::Network *net, const nemo::Configuration& conf, int argc, char* argv [], unsigned duration, bool timed, const std::string& file);
	~SimulationMPI();
private:
	Timer timer;
	unsigned neurons, synapses;
	void simulateTimed(nemo::mpi_dist::MasterSimulation& master, unsigned duration, std::ostream& out);
	void simulateStepped(nemo::mpi_dist::MasterSimulation& master, unsigned duration, std::ostream& out);
};

	}
}
#endif
