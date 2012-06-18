#ifndef MASTER_SIM_HPP
#define MASTER_SIM_HPP
#include <nemo/Network.hpp>
#include <nemo/Configuration.hpp>
#include <nemo/Timer.hpp>
#include <vector>
#include "MapperSim.hpp"
#include "mpi.h"

namespace nemo {
	namespace mpi_dist {

class MasterSimulation {

	public :
		/* The constructor takes into itself both network and configuration, as well as set of variables that are used for setting up the simulation */
		MasterSimulation(const nemo::Network &net, const nemo::Configuration& conf);
		~MasterSimulation();

		/* Return neuron count of the net */
		unsigned getNeuronCount();

		/* Simulation step 
			returns the number of neurons fired */
		unsigned step();

		/* Simulation step with external stimulus
			returns the number of neurons fired */
		unsigned step(std::vector<unsigned>& stimulus);
		
		/* Finish simulation
			returns total number of spikes */
		unsigned endSimulation();

	private :

		/* MPI communication status */
		MPI::Status status;

		/* Set of variables for the calculations some are set to be accessed by the MapperSim class */
		unsigned workers, neuronCount;
		bool verbose;

		/* Distribution functions 
			1) During first few seconds of the creation, Master allocates the neurons to the corresponding nodes via the use of MapperSim
			2) The configuration that has been passed to Master is transmitted to the workers
			3) Neuron data is retrieved from the network and passed to the corresponding workers
			4) Finally, the synapses are distributed to the workers
		*/
		void distributeMapper(nemo::mpi_dist::MapperSim& mapper);
		void distributeConfiguration(const nemo::Configuration& conf);
		void distributeNeurons(const nemo::Network& net, MapperSim& mapper);
		void distributeSynapses(const nemo::network::NetworkImpl& net, const MapperSim& mapper);
		
		/* Helper function to get the data from the network */
		float* getNeuronIdx(unsigned idx, const nemo::Network& net);

		/* Helper functions for the data transmission */
		void sendData(const std::string& data, unsigned length_tag, unsigned data_tag, unsigned worker);
		void sendSynapseData(const std::string& data, unsigned worker);

};

	} // namespace mpi_dist
} // namespace nemo
#endif
