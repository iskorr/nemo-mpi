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

		WorkerSimulation(unsigned rank, unsigned workers);
		~WorkerSimulation();

	private:
		
		MPI::Status status;
		MPI::Request recv_request, send_request;
		MapperSim mapper;
		unsigned rank, reply, neuronCount, workers, outfirings, spikes;
		std::vector <std::vector <unsigned> > outgoingSynapses;
		std::vector <std::pair <unsigned, std::vector <inc_syn> > > incomingSynapses;
		std::vector <std::pair <unsigned, float> > stim_template;
		std::deque <int> incoming;
		
		void receiveMapper();
		void receiveConfiguration(nemo::Configuration& conf);
		void receiveNeurons(nemo::Network* net);
		void receiveSynapses(nemo::Network* net);

		std::vector <inc_syn> getSynapseData(unsigned source);
		void assignSynapse(nemo::Network* net, std::vector<std::string> params);
		void addIncomingSynapse(unsigned source, unsigned target, unsigned delay, float weight);
		void addOutgoingSynapse(unsigned source, unsigned target);

		void runSimulation(nemo::Simulation* sim);
		void enqueueIncomingSpikes(nemo::Simulation* sim, std::vector <std::pair<unsigned,float> >& stimulus);
		void distributeOutgoingSpikes(const std::vector <unsigned>& firing_output);

};

	}
}
#endif	
