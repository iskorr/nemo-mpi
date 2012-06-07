#define MASTER 0
#define CONFIGURATION_LENGTH_TAG 0
#define CONFIGURATION_DATA_TAG 1
#define NEURON_COUNT_TAG 2
#define NEURON_LENGTH_TAG 3
#define NEURON_DATA_TAG 4
#define SYNAPSE_LENGTH_TAG 5
#define SYNAPSE_DATA_TAG 6
#include "MasterSimulation.hpp"
#include "mpi.h"
#include <iostream>
#include <vector>
#include "parsing.hpp"

using namespace std;

MasterSimulation::MasterSimulation(const Network& net, const Configuration& conf)
{
	workers = MPI::COMM_WORLD.Get_size();
	unsigned ncount = net.neuronCount();
	mapper(ncount, workers-1, MASTER);
	
	// Setting up the network
	distributeConfiguration(conf);
	distributeNeurons(net);
	distributeSynapses();
}

void
MasterSimulation::distributeConfiguration(Configuration& conf)
{
	string config = encodeConfiguration(conf);
	unsigned strlen = config.size()+1;
	char msg [strlen];
	msg[strlen-1]=0;
	memcpy(msg,neurons[nidx].c_str(),strlen-1);
	for(unsigned worker = 1; worker < workers; ++worker) {
		MPI::COMM_WORLD.Send(&strlen, 1, MPI::INT, worker, CONFIGURATION_LENGTH_TAG);
		MPI::COMM_WORLD.Send(&msg, strlen, MPI::CHAR, worker, CONFIGURATION_DATA_TAG);
	}
}

void 
MasterSimulation::distributeNeurons(const Network& net)
{
/*	for (unsigned worker = 1; worker < workers; ++worker) {
		vector<neuron_data> neurons = simMapper.retrieveNeurons(worker);
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
	string neuronData;
	for (unsigned worker = 1; worker < workers; ++worker) {
		MPI::COMM_WORLD.Send(&neuronsPerWorker, 1, MPI::INT, worker, NEURON_COUNT_TAG, status);
		for (unsigned nidx = 0; nidx < neuronsPerWorker; ++nidx) {
			neuronData = encodeNeuron(getNeuron(nidx+(worker-1)*neuronsPerWorker, net));
			unsigned strlen = neuronData.size()+1;
			char msg [strlen];
			msg[strlen-1]=0;
			memcpy(msg,neuronData.c_str(),strlen-1);
			MPI::COMM_WORLD.Send(&strlen, 1, MPI::INT, worker, NEURON_LENGTH_TAG);
			MPI::COMM_WORLD.Send(&msg, strlen, MPI::CHAR, worker, NEURON_DATA_TAG);
		}
	}
}

void
MasterSimulation::distributeSynapses()
{
	
	/* PSEUDOCODE
	vector <Synapse> synapses = retrieveSynapses();
	unsigned source, target, synLength;
	string synData;
	for (unsigned worker = 1; worker < workers; ++worker) {
		for (Synapse s : targetWithinWorker) {
			synData = encodeSynapse(s);
			synLength = synData.size()+1;
			char msg [strlen];
			msg[strlen-1] = 0;
			memcpy(msg, synData.c_str(), strlen-1);
			MPI::COMM_WORLD.Send(&strlen, 1, MPI::INT, worker, SYNAPSE_LENGTH_TAG);
			MPI::COMM_WORLD.Send(&msg, strlen, MPI::CHAR, worker, SYNAPSE_DATA_TAG);
		}
	}	
	*/
}

float*
getNeuron(unsigned idx, const Network& net)
{
	float res = new float [7];
	for (unsigned i = 0; i < 3; ++i) res[i] = net.getNeuronState(idx,i);
	res[3] = net.getNeuronParameter(idx,0);
	res[4] = net.getNeuronParameter(idx,1);
	res[5] = net.getNeuronState(idx,4);
	res[6] = idx;
	return res;
}
