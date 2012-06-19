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

MasterSimulation::MasterSimulation(const nemo::Network& net, const nemo::Configuration& conf) : workers(MPI::COMM_WORLD.Get_size()), verbose(true)
{
	if(verbose) cout << "Master initialised" << endl;
	unsigned ok, worker = 1;
	if(verbose) cout << "Clustering...";
	MapperSim mapper(net, workers-1);
	if(verbose) cout << "................complete" << endl;
	
	distributeMapper(mapper);
	distributeConfiguration(conf);
	distributeNeurons(net, mapper);
	distributeSynapses(*net.m_impl, mapper);

	for (; worker < workers; ++worker) {
		MPI::COMM_WORLD.Recv(&ok, 1, MPI::INT, worker, DISTRIBUTION_COMPLETE, status);
		if (ok == 1) break;
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
	for(unsigned worker = 1; worker < workers; ++worker) {
		MPI::COMM_WORLD.Send(&strlen, 1, MPI::INT, worker, CONFIGURATION_LENGTH_TAG);
		MPI::COMM_WORLD.Send(config.c_str(), strlen, MPI::CHAR, worker, CONFIGURATION_DATA_TAG);
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
			MPI::COMM_WORLD.Send(getNeuronIdx(workerNeurons[nidx], net), 8, MPI::FLOAT, worker, NEURON_DATA_TAG);
		}
	}
	if(verbose) cout << ".......complete" << endl;
}

void
MasterSimulation::distributeSynapses(const nemo::network::NetworkImpl& net, const MapperSim& mapper)
{
	if(verbose) cout << "Synapses distribution...";
	unsigned source,target;
	float* encodedSynapse;
	for(nemo::network::synapse_iterator s = net.synapse_begin(); s != net.synapse_end(); ++s) {
		source = mapper.rankOf(s->source);
		target = mapper.rankOf(s->target());
		encodedSynapse = encodeSynapse(s->source, s->target(), s->delay, s->weight(), s->plastic() == 0? 0 : 1);
		if(source == target) {
			sendSynapseData(encodedSynapse, target);
		} else {
			sendSynapseData(encodedSynapse, source);
			sendSynapseData(encodedSynapse, target);
		}
	}
	encodedSynapse = encodeSynapse(-1,0,0,0,0);
	for (unsigned worker = 1; worker < workers; ++worker) sendSynapseData(encodedSynapse,worker);
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
	MPI::COMM_WORLD.Send(&strlen, 1, MPI::INT, worker, length_tag);
	MPI::COMM_WORLD.Send(data.c_str(), strlen, MPI::CHAR, worker, data_tag);
}

void
MasterSimulation::sendSynapseData(float* encodedSynapse, unsigned worker)
{
	MPI::COMM_WORLD.Send(encodedSynapse, SYNAPSE_ARGS, MPI::FLOAT, worker, SYNAPSE_DATA_TAG);
}

	} // namespace mpi_dist
} // namespace nemo
