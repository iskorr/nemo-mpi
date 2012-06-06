#define MASTER 0
#define NEURON_COUNT_TAG 0
#define NEURON_LENGTH_TAG 1
#define NEURON_DATA_TAG 2
#include "MasterSimulation.hpp"
#include "mpi.h"
#include <iostream>

using namespace std;

MasterSimulation::MasterSimulation(const Network& net, const Configuration& conf)
{
	unsigned workers = MPI::COMM_WORLD.Get_size();
	unsigned ncount = net.neuronCount();
	mapper(ncount, workers-1, MASTER);
	
	// Setting up the network
	distributeNeurons();
	distributeSynapses();
}

void 
MasterSimulation::distributeNeurons()
{
/*	for (unsigned worker = 1; worker < workers; ++worker) {
		vector<neuron_data> neurons = mapper.retrieveNeurons(worker, net);
		unsigned ncount = neurons.size();
		MPI::COMM_WORLD.Send(&ncount, 1, MPI::INT, worker, NEURON_COUNT_TAG);
		for (unsigned nidx = 0; nidx < ncount; ++nidx) {
			unsigned strlen = neurons[nidx].size()+1;
			char msg [strlen];
			msg[strlen-1]=0;
			memcpy(msg,neurons[nidx].c_str(),strlen-1);
			MPI::COMM_WORLD.Send(&strlen, 1, MPI::INT, worker, NEURON_LENGTH_TAG);
			MPI::COMM_WORLD.Send(&msg, strlen, MPI::CHAR, worker, NEURON_DATA_TAG);
		}
	}*/
	unsigned neuronsPerWorker = mapper.nodeSize();
	for (unsigned worker = 1; worker < workers; ++worker) {
		MPI::COMM_WORLD.Send(&neuronsPerWorker, 1, MPI::INT, worker, (int) 0, status);
		for (unsigned nidx = 0; nidx < neuronsPerWorker; ++nidx) {
			
		}
	}
}

void MasterSimulation::distributeSynapses()
{
	
}
