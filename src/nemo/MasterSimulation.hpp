#ifndef MASTER_SIM_HPP
#define MASTER_SIM_HPP
#include <nemo/Network.hpp>
#include <nemo/Configuration.hpp>
#include <nemo/network/Generator.hpp>
#include "MapperSim.hpp"
#include "mpi.h"
#include <nemo.h>

class MasterSimulation {

	public :
		
		MasterSimulation(const nemo::Network &net, const nemo::Configuration& conf);
		~MasterSimulation();

	private :

		MPI::Status status;
		unsigned workers;
		void distributeConfiguration(const nemo::Configuration& conf);
		void distributeNeurons(const nemo::Network& net, const MapperSim& mapper);
		void distributeSynapses(const nemo::network::NetworkImpl& net, const MapperSim& mapper);
		float* getNeuronIdx(unsigned idx, const nemo::Network& net);

};
#endif
