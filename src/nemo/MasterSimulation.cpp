#include "mpi.h"
#include "MasterSimulation.hpp"
#include "mpi_tags.hpp"
#include <iostream>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <nemo/parsing.hpp>
#include <nemo/Network.hpp>
#include <nemo/Configuration.hpp>

using namespace std;

MasterSimulation::MasterSimulation(const nemo::Network& net, const nemo::Configuration& conf)
{
	workers = MPI::COMM_WORLD.Get_size();
	unsigned ok,ncount = net.neuronCount();
	MapperSim mapper(ncount, workers-1);
	
	// Setting up the network
	cout << "Start Master" << endl;
	distributeConfiguration(conf);
	cout << "Conf done Master" << endl;
	distributeNeurons(net, mapper);
	cout << "Neurons done Master" << endl;
	distributeSynapses(*net.m_impl, mapper);
	cout << "Synapses done Master" << endl;

	for (unsigned worker = 1; worker < workers; ++workers) {
		MPI::COMM_WORLD.Recv(&ok, 1, MPI::INT, worker, DISTRIBUTION_COMPLETE, status);
	}
	cout << "All the workers are set up for simulation" << endl;
}

MasterSimulation::~MasterSimulation()
{
	;
}

void
MasterSimulation::distributeConfiguration(const nemo::Configuration& conf)
{
	string config = encodeConfiguration(conf);
	unsigned strlen = config.size()+1;
	char msg [strlen];
	msg[strlen-1]=0;
	memcpy(msg,config.c_str(),strlen-1);
	for(unsigned worker = 1; worker < workers; ++worker) {
		MPI::COMM_WORLD.Send(&strlen, 1, MPI::INT, worker, CONFIGURATION_LENGTH_TAG);
		MPI::COMM_WORLD.Send(&msg, strlen, MPI::CHAR, worker, CONFIGURATION_DATA_TAG);
	}
}

void 
MasterSimulation::distributeNeurons(const nemo::Network& net, const MapperSim& mapper)
{
	unsigned neuronsPerWorker = mapper.workerSize();
	string neuronData;
	cout << "START" << endl;

	for (unsigned worker = 1; worker < workers; ++worker) {
		/* Clustering mapping will go here:
			vector<unsigned> workerNeurons = mapper.retrieveNeurons(worker);
			neuronsPerWorker = workerNeurons.size();
		*/
		MPI::COMM_WORLD.Send(&neuronsPerWorker, 1, MPI::INT, worker, NEURON_COUNT_TAG);
		for (unsigned nidx = 0; nidx < neuronsPerWorker; ++nidx) {
			neuronData = encodeNeuron(getNeuronIdx(nidx+(worker-1)*neuronsPerWorker, net));
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
MasterSimulation::distributeSynapses(const nemo::network::NetworkImpl& net, const MapperSim& mapper)
{
	string encodedSynapse;
	unsigned source,target,synLength,ok = 0;
	for(nemo::network::synapse_iterator s = net.synapse_begin(); s != net.synapse_end(); ++s) {
		source = mapper.rankOf(s->source);
		target = mapper.rankOf(s->target());
		if(source != target) {
			encodedSynapse = encodeSynapse(s,true);
		} else {
			encodedSynapse = encodeSynapse(s,false);
		}
		synLength = encodedSynapse.size()+1;
		char msg [synLength];
		msg[synLength-1] = 0;
		memcpy(msg, encodedSynapse.c_str(), synLength-1);
		MPI::COMM_WORLD.Send(&ok, 1, MPI::INT, source, SYNAPSE_END_TAG);
		MPI::COMM_WORLD.Send(&synLength, 1, MPI::INT, source, SYNAPSE_LENGTH_TAG);
		MPI::COMM_WORLD.Send(&msg, synLength, MPI::CHAR, source, SYNAPSE_DATA_TAG);
	}
	ok = 1;
	for (unsigned worker = 1; worker < workers; ++worker) MPI::COMM_WORLD.Send(&ok, 1, MPI::INT, worker, SYNAPSE_END_TAG);
}

float*
MasterSimulation::getNeuronIdx(unsigned idx, const nemo::Network& net)
{
	cout << "STATTS" << endl;
	float* res = new float [7];
	cout << "STATTS" << endl;
	for (unsigned i = 0; i < 3; ++i) res[i] = net.getNeuronState(idx,i);
	res[3] = net.getNeuronParameter(idx,0);
	res[4] = net.getNeuronParameter(idx,1);
	res[5] = net.getNeuronState(idx,4);
	res[6] = idx;
	for (unsigned i = 0; i < 7; ++i) cout << res[i] << endl;
	return res;
}
