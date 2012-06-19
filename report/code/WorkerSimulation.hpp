#ifndef WORKER_SIM_HPP
#define WORKER_SIM_HPP
#include "mpi.h"
#include "MapperSim.hpp"
#include <nemo/Network.hpp>
#include <nemo/Configuration.hpp>
#include <string>
#include <queue>
#include <deque>

namespace nemo {
	namespace mpi_dist {

struct inc_syn
{
        unsigned target;
        unsigned delay;
        float weight;
};

class WorkerSimulation {

	public:

		/* The constructor is passed the number of other workers and its own rank */
		WorkerSimulation(unsigned rank, unsigned workers);
		~WorkerSimulation();

	private:
		
		/* MPI Communication capabilities */
		MPI::Status status;
		MPI::Request recv_request, send_request;
		unsigned sent, received;

		/* Mapper for the network */
		MapperSim mapper;
		
		/* Set of variables - most of it for data-logging purposes */
		unsigned rank, reply, neuronCount, workers, fired, spikes, spikesPerStep, firedPerStep;

		/* In order to know where to send the spike data worker needs a list of outgoing synapses with corresponding neuron - list of targets pairs*/
		std::vector <std::vector <unsigned> > outgoingSynapses;

		/* For incoming spikes - now there are triples with information - including delay and weight */
		std::vector <std::pair <unsigned, std::vector <inc_syn> > > incomingSynapses;
	
		/* Template for creation of a vector of external stimulus */
		std::vector <std::pair <unsigned, float> > stim_template;

		/* Queues for dealing with the incoming spikes */
		std::deque <int> incoming;
		std::deque <std::deque <std::pair<unsigned, float> > > delay_queue;
		
		/* Receiving functions
			1) Receive Mapper layout
			1) Receive configuration data sent by the master
			2) Receive neuron data - populate the local network
			4) Receive synapse data - depending on the source and target info received, decide whether it is a local, incoming or outgoing synapse */
		void receiveMapper();
		void receiveConfiguration(nemo::Configuration& conf);
		void receiveNeurons(nemo::Network* net);
		void receiveSynapses(nemo::Network* net);

		/* Auxiliary functions for synapse distribution */
		std::vector <inc_syn> getSynapseData(unsigned source);
		void assignSynapse(nemo::Network* net, float* params);
		void addIncomingSynapse(unsigned source, unsigned target, unsigned delay, float weight);
		void addOutgoingSynapse(unsigned source, unsigned target);

		/* Simulation with auxiliary functions */
		void runSimulation(nemo::Simulation* sim);
		void enqueueIncomingSpikes(nemo::Simulation* sim, std::vector <std::pair<unsigned,float> >& stimulus);
		void distributeOutgoingSpikes(const std::vector <unsigned>& firing_output);

};

	}
}
#endif	
