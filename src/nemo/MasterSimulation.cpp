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
#include <nemo/Timer.hpp>

using namespace std;

MasterSimulation::MasterSimulation(const nemo::Network& net, const nemo::Configuration& conf, unsigned duration)
{
	workers = MPI::COMM_WORLD.Get_size();
	unsigned ok, worker = 1, ncount = net.neuronCount();
	MapperSim mapper(ncount, workers-1);
	
	// Setting up the network
	cout << "Master initialised" << endl;
	distributeConfiguration(conf);
	cout << "Configuration distribution complete" << endl;
	distributeNeurons(net, mapper);
	cout << "Neuron distribution complete" << endl;	
	distributeSynapses(*net.m_impl, mapper);
	cout << "Synapses distribution complete" << endl;

	for (; worker < workers; ++worker) {
		MPI::COMM_WORLD.Recv(&ok, 1, MPI::INT, worker, DISTRIBUTION_COMPLETE, status);
		if (ok == 0) break;
	}
	if (worker >= workers-1) cout << "All the workers are set up for simulation" << endl;
	else cout << "There is a problem with a worker " << worker << endl;
	MPI_Barrier(MPI_COMM_WORLD);
	simulate(duration);
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
	for (unsigned worker = 1; worker < workers; ++worker) {
		/* Clustering mapping will go here:
			vector<unsigned> workerNeurons = mapper.retrieveNeurons(worker);
			neuronsPerWorker = workerNeurons.size();
		*/
		MPI::COMM_WORLD.Send(&neuronsPerWorker, 1, MPI::INT, worker, NEURON_COUNT_TAG);
		for (unsigned nidx = 0; nidx < neuronsPerWorker; ++nidx) {
			neuronData = encodeNeuron(getNeuronIdx(nidx+(worker-1)*neuronsPerWorker, net),nidx);
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
	string encodedSynapse,sourceSynapse;
	unsigned source,target,synLength,ok = 0;
	for(nemo::network::synapse_iterator s = net.synapse_begin(); s != net.synapse_end(); ++s) {
		source = mapper.rankOf(s->source);
		target = mapper.rankOf(s->target());
		if(source == target) {
			encodedSynapse = encodeSynapse(s,1);
		} else {
			encodedSynapse = encodeSynapse(s,2);
			sourceSynapse = encodeSynapse(s,3);
			synLength = sourceSynapse.size()+1;
			char msg [synLength];
			msg[synLength-1] = 0;
			memcpy(msg, sourceSynapse.c_str(), synLength-1);
			MPI::COMM_WORLD.Send(&ok, 1, MPI::INT, source, SYNAPSE_END_TAG);
			MPI::COMM_WORLD.Send(&synLength, 1, MPI::INT, source, SYNAPSE_LENGTH_TAG);
			MPI::COMM_WORLD.Send(&msg, synLength, MPI::CHAR, source, SYNAPSE_DATA_TAG);
		}
		synLength = encodedSynapse.size()+1;
		char msg [synLength];
		msg[synLength-1] = 0;
		memcpy(msg, encodedSynapse.c_str(), synLength-1);
		MPI::COMM_WORLD.Send(&ok, 1, MPI::INT, target, SYNAPSE_END_TAG);
		MPI::COMM_WORLD.Send(&synLength, 1, MPI::INT, target, SYNAPSE_LENGTH_TAG);
		MPI::COMM_WORLD.Send(&msg, synLength, MPI::CHAR, target, SYNAPSE_DATA_TAG);
	}
	ok = 1;
	for (unsigned worker = 1; worker < workers; ++worker) MPI::COMM_WORLD.Send(&ok, 1, MPI::INT, worker, SYNAPSE_END_TAG);
}

float*
MasterSimulation::getNeuronIdx(unsigned idx, const nemo::Network& net)
{
	float* res = new float [8];
	for (unsigned i = 0; i < 4; ++i) res[i] = net.getNeuronParameter(idx,i);
	res[4] = net.getNeuronState(idx,0);
	res[5] = net.getNeuronState(idx,1);
	res[6] = net.getNeuronParameter(idx,4);
	res[7] = idx;
	return res;
}

void
MasterSimulation::simulate(unsigned duration)
{
	cout << "Simulation started" << endl;
	unsigned step = 0, stepOK = 0, stepDONE;
	nemo::Timer timer;
	timer.reset();
	while(timer.elapsedWallclock() < duration) {
		MPI::COMM_WORLD.Bcast(&stepOK, 1, MPI::INT, MASTER);
		for (unsigned worker = 1; worker < workers; ++worker) MPI::COMM_WORLD.Recv(&stepDONE, 1, MPI::INT, worker, SIM_STEP, status);
		step++;
	}
	stepOK = 1;
	MPI::COMM_WORLD.Bcast(&stepOK, 1, MPI::INT, MASTER);
	cout << "Total of " << step << " steps succeded" << endl;
}
