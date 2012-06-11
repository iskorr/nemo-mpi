#ifndef WORKER_SIM_HPP
#define WORKER_SIM_HPP
#include "mpi.h"
#include "MapperSim.hpp"
#include <nemo/Network.hpp>
#include <nemo/Configuration.hpp>
#include <string>
#include <queue>
#include <deque>
using namespace std;

namespace nemo {
	namespace mpi_dist {

class WorkerSimulation {

	public:

		WorkerSimulation(unsigned rank, unsigned workers);
		~WorkerSimulation();

	private:
		
		MPI::Status status;
		MPI::Request recv_request, send_request;
		unsigned rank, reply, neuronCount, workers;
		vector <vector <unsigned> > outgoingSynapses;
		vector <vector <vector <float> > > incomingSynapses;
		vector <pair <unsigned, float> > stim_template;
		deque <int> incoming;
		
		void receiveConfiguration(nemo::Configuration& conf);
		void receiveNeurons(nemo::Network* net);
		void receiveSynapses(nemo::Network* net);

		vector <vector <float> > getSynapseData(unsigned source);
		void assignSynapse(nemo::Network* net, vector<string> params);
		void addIncomingSynapse(unsigned source, unsigned target, unsigned delay, float weight);
		void addOutgoingSynapse(unsigned source, unsigned target);

		void runSimulation(nemo::Simulation* sim);
		void enqueueIncomingSpikes(nemo::Simulation* sim, vector <pair<unsigned,float> >& stimulus);
		void distributeOutgoingSpikes(const vector <unsigned>& firing_output);

		unsigned mapLocal(unsigned neuron);
		unsigned mapGlobal(unsigned neuron);
};

	}
}
#endif	
