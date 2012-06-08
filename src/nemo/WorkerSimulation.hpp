#ifndef WORKER_SIM_HPP
#define WORKER_SIM_HPP
#include "mpi.h"
#include <nemo/Network.hpp>
#include <nemo/Configuration.hpp>

class WorkerSimulation {

	public:

		WorkerSimulation();
		~WorkerSimulation();

	private:
		
		MPI::Status status;
		MPI::Request request;
		unsigned rank;
		void receiveConfiguration(nemo::Configuration& conf);
		void receiveNeurons(nemo::Network* net);
		void receiveSynapses(nemo::Network* net);
};
#endif	
