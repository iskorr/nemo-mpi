#ifndef MASTER_SIM_HPP
#define MASTER_SIM_HPP
#include <nemo/Network.hpp>
#include <nemo/Configuration.hpp>
#include "MapperSim.hpp"
#include "mpi.h"

namespace nemo {
	namespace mpi_dist {

class MasterSimulation {

	public :
		
		MasterSimulation(const nemo::Network &net, const nemo::Configuration& conf, unsigned duration, bool timed, const std::string& filename);
		~MasterSimulation();

	private :

		MPI::Status status;
		unsigned workers, neuronCount;
		bool verbose;
		void distributeMapper(nemo::mpi_dist::MapperSim& mapper);
		void distributeConfiguration(const nemo::Configuration& conf);
		void distributeNeurons(const nemo::Network& net, MapperSim& mapper);
		void distributeSynapses(const nemo::network::NetworkImpl& net, const MapperSim& mapper);
		float* getNeuronIdx(unsigned idx, const nemo::Network& net);
		void simulate(unsigned duration, bool timed, const std::string& filename);

};

	}
}
#endif
