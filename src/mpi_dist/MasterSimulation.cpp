#include "MasterSimulation.hpp"
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mpi_tags.hpp"
#include "parsing.hpp"
#include <nemo/Timer.hpp>
using namespace std;

namespace nemo {
	namespace mpi_dist {

MasterSimulation::MasterSimulation(const nemo::Network& net, const nemo::Configuration& conf) : workers(MPI::COMM_WORLD.Get_size()), neuronCount(net.neuronCount()), verbose(true)
{
	unsigned ok, worker = 1;
	MapperSim mapper(net, workers-1);
	
	if(verbose) cout << "Master initialised" << endl;
	distributeMapper(mapper);
	distributeConfiguration(conf);
	distributeNeurons(net, mapper);
	distributeSynapses(*net.m_impl, mapper);

	for (; worker < workers; ++worker) {
		MPI::COMM_WORLD.Recv(&ok, 1, MPI::INT, worker, DISTRIBUTION_COMPLETE, status);
		if (ok == 0) break;
	}
	if(verbose) {
		if (worker >= workers-1) cout << "All the workers are set up for simulation" << endl;
		else cout << "There is a problem with a worker " << worker << endl;
	}
	MPI_Barrier(MPI_COMM_WORLD);
}

MasterSimulation::~MasterSimulation()
{
	;
}

void
MasterSimulation::distributeMapper(nemo::mpi_dist::MapperSim& mapper)
{
	if(verbose) cout << "Mapper distribution...";
	unsigned neurons = mapper.neuronCount();
	vector<string> mapSet = encodeMapper(mapper);
	for (unsigned worker = 1; worker < workers; ++worker) {
		MPI::COMM_WORLD.Send(&neurons, 1, MPI::INT, worker, TOTAL_NEURONS_TAG);
		for (unsigned map = 0; map < mapSet.size(); ++map) {
			sendData(mapSet[map], MAPPER_LENGTH_TAG, MAPPER_DATA_TAG, worker);
		}
	}
	if(verbose) cout << ".......complete" << endl;
}

void
MasterSimulation::distributeConfiguration(const nemo::Configuration& conf)
{
	if(verbose) cout << "Configuration distribution...";
	string config = encodeConfiguration(conf);
	unsigned strlen = config.size()+1;
	char msg [strlen];
	msg[strlen-1]=0;
	memcpy(msg,config.c_str(),strlen-1);
	for(unsigned worker = 1; worker < workers; ++worker) {
		MPI::COMM_WORLD.Send(&strlen, 1, MPI::INT, worker, CONFIGURATION_LENGTH_TAG);
		MPI::COMM_WORLD.Send(&msg, strlen, MPI::CHAR, worker, CONFIGURATION_DATA_TAG);
	}
	if(verbose) cout << "complete" << endl;
}

void 
MasterSimulation::distributeNeurons(const nemo::Network& net, MapperSim& mapper)
{
	if(verbose) cout << "Neuron distribution...";
	unsigned neuronsPerWorker;
	for (unsigned worker = 1; worker < workers; ++worker) {
		vector<unsigned> workerNeurons = mapper.retrieveNeurons(worker);
		neuronsPerWorker = workerNeurons.size();
		MPI::COMM_WORLD.Send(&neuronsPerWorker, 1, MPI::INT, worker, NEURON_COUNT_TAG);
		for (unsigned nidx = 0; nidx < neuronsPerWorker; ++nidx) {
			sendData(encodeNeuron(getNeuronIdx(workerNeurons[nidx], net),workerNeurons[nidx]), NEURON_LENGTH_TAG, NEURON_DATA_TAG, worker);
		}
	}
	if(verbose) cout << ".......complete" << endl;
}

void
MasterSimulation::distributeSynapses(const nemo::network::NetworkImpl& net, const MapperSim& mapper)
{
	if(verbose) cout << "Synapses distribution...";
	unsigned source,target,ok = 0;
	for(nemo::network::synapse_iterator s = net.synapse_begin(); s != net.synapse_end(); ++s) {
		source = mapper.rankOf(s->source);
		target = mapper.rankOf(s->target());
		if(source == target) {
			sendSynapseData(encodeSynapse(s,1), target);
		} else {
			sendSynapseData(encodeSynapse(s,2), target);
			sendSynapseData(encodeSynapse(s,3), source);
		}
	}
	ok = 1;
	for (unsigned worker = 1; worker < workers; ++worker) MPI::COMM_WORLD.Send(&ok, 1, MPI::INT, worker, SYNAPSE_END_TAG);
	if(verbose) cout << ".....complete" << endl;
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

unsigned
MasterSimulation::step()
{
	unsigned stepDONE, stepOK = 0, firingPerStep = 0;
	MPI::COMM_WORLD.Bcast(&stepOK, 1, MPI::INT, MASTER);
	for (unsigned worker = 1; worker < workers; ++worker) {
		MPI::COMM_WORLD.Recv(&stepDONE, 1, MPI::INT, worker, SIM_STEP, status);
		firingPerStep += stepDONE;
	}
	return firingPerStep;
}

unsigned
MasterSimulation::step(vector<unsigned>& stimulus)
{
	unsigned stepDONE, stepOK = 1, firingPerStep = 0;
	MPI::COMM_WORLD.Bcast(&stepOK, 1, MPI::INT, MASTER);
	for (unsigned worker = 1; worker < workers; ++worker) {
		MPI::COMM_WORLD.Recv(&stepDONE, 1, MPI::INT, worker, SIM_STEP, status);
		firingPerStep += stepDONE;
	}
	return firingPerStep;
}

unsigned
MasterSimulation::endSimulation()
{
	unsigned buf, stepOK = 2, totalspikes = 0;
	MPI::COMM_WORLD.Bcast(&stepOK, 1, MPI::INT, MASTER);
	for (unsigned worker = 1; worker < workers; ++worker) {
		MPI::COMM_WORLD.Recv(&buf, 1, MPI::INT, worker, SPIKES, status);
		totalspikes+= buf;
	}
	return totalspikes;
}

void
MasterSimulation::sendData(const string& data, unsigned length_tag, unsigned data_tag, unsigned worker)
{
	unsigned strlen = data.size()+1;
	char msg [strlen];
	msg[strlen-1]=0;
	memcpy(msg,data.c_str(),strlen-1);
	MPI::COMM_WORLD.Send(&strlen, 1, MPI::INT, worker, length_tag);
	MPI::COMM_WORLD.Send(&msg, strlen, MPI::CHAR, worker, data_tag);
}

void
MasterSimulation::sendSynapseData(const string& data, unsigned worker)
{
	unsigned ok = 0,strlen = data.size()+1;
	char msg [strlen];
	msg[strlen-1]=0;
	memcpy(msg,data.c_str(),strlen-1);
	MPI::COMM_WORLD.Send(&ok, 1, MPI::INT, worker, SYNAPSE_END_TAG);
	MPI::COMM_WORLD.Send(&strlen, 1, MPI::INT, worker, SYNAPSE_LENGTH_TAG);
	MPI::COMM_WORLD.Send(&msg, strlen, MPI::CHAR, worker, SYNAPSE_DATA_TAG);
}


unsigned
MasterSimulation::getNeuronCount()
{
	return neuronCount;
}


	} // namespace mpi_dist
} // namespace nemo
