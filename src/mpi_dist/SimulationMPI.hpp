/* Copyright 2012 Imperial College London */
/* Overall simulation class - can be used instead of original Simulation class - though needs some extra inputs */
#ifndef MPI_SIMULATION_HPP
#define MPI_SIMULATION_HPP
#include "MasterSimulation.hpp"
#include "WorkerSimulation.hpp"
#include <nemo/Timer.hpp>

namespace nemo {
	namespace mpi_dist {

class SimulationMPI {
public:
	/* The constructor, taking:
	 * main arguments - for MPI instantiation,
	 * duration of the simulation - depending on the timed variable, number of simulation milliseconds or steps
	 * output file name - for written output
	 */
	SimulationMPI(const nemo::Network *net, const nemo::Configuration& conf, int argc, char* argv [], unsigned duration, bool timed, const std::string& file);
	~SimulationMPI();
private:
	/* Built-in timer, for the simulation */
	Timer timer;
	/* Inner variables for output */
	unsigned neurons, synapses;
	/* Simulation of timed version - in milliseconds */
	void simulateTimed(nemo::mpi_dist::MasterSimulation& master, unsigned duration, std::ostream& out);
	/* Simulation of stepped version - in simulation steps */
	void simulateStepped(nemo::mpi_dist::MasterSimulation& master, unsigned duration, std::ostream& out);
};

	}
}
#endif
