#include "MasterSimulation.hpp"
#include <iostream>
#include <fstream>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mpi_tags.hpp"
#include "parsing.hpp"
#include <nemo/Timer.hpp>
using namespace std;

namespace nemo {
	namespace mpi_dist {

MasterSimulation::MasterSimulation(const nemo::Network& net, const nemo::Configuration& conf, unsigned duration, bool timed, const string& filename) : 
									workers(MPI::COMM_WORLD.Get_size()),
									neuronCount(net.neuronCount()),
									verbose(false)
{
	unsigned ok, worker = 1;
	MapperSim mapper(net, workers-1);
	
	// Setting up the network
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
	simulate(duration, timed, filename);
}

MasterSimulation::~MasterSimulation()
{
	;
}

void
MasterSimulation::distributeMapper(nemo::mpi_dist::MapperSim& mapper)
{
	if(verbose) cout << "Mapper distribution...";
	unsigned strlen, neurons = mapper.neuronCount();
	vector<string> mapSet = encodeMapper(mapper);
	for (unsigned worker = 1; worker < workers; ++worker) {
		MPI::COMM_WORLD.Send(&neurons, 1, MPI::INT, worker, TOTAL_NEURONS_TAG);
		for (unsigned map = 0; map < mapSet.size(); ++map) {
			strlen = mapSet[map].size()+1;
			char msg [strlen];
			msg[strlen-1]=0;
			memcpy(msg,mapSet[map].c_str(),strlen-1);
			MPI::COMM_WORLD.Send(&strlen, 1, MPI::INT, worker, MAPPER_LENGTH_TAG);
			MPI::COMM_WORLD.Send(&msg, strlen, MPI::CHAR, worker, MAPPER_DATA_TAG);
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
	string neuronData;
	for (unsigned worker = 1; worker < workers; ++worker) {
		vector<unsigned> workerNeurons = mapper.retrieveNeurons(worker);
		neuronsPerWorker = workerNeurons.size();
		MPI::COMM_WORLD.Send(&neuronsPerWorker, 1, MPI::INT, worker, NEURON_COUNT_TAG);
		for (unsigned nidx = 0; nidx < neuronsPerWorker; ++nidx) {
			neuronData = encodeNeuron(getNeuronIdx(workerNeurons[nidx], net),workerNeurons[nidx]);
			unsigned strlen = neuronData.size()+1;
			char msg [strlen];
			msg[strlen-1]=0;
			memcpy(msg,neuronData.c_str(),strlen-1);
			MPI::COMM_WORLD.Send(&strlen, 1, MPI::INT, worker, NEURON_LENGTH_TAG);
			MPI::COMM_WORLD.Send(&msg, strlen, MPI::CHAR, worker, NEURON_DATA_TAG);
		}
	}
	if(verbose) cout << ".......complete" << endl;
}

void
MasterSimulation::distributeSynapses(const nemo::network::NetworkImpl& net, const MapperSim& mapper)
{
	if(verbose) cout << "Synapses distribution...";
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

void
MasterSimulation::simulate(unsigned duration, bool timed, const string& filename)
{
	if(verbose) cout << "Simulation started" << endl;
	unsigned stepOK = 0, stepDONE, firingPerStep;
	unsigned long runtime = 0;
	nemo::Timer timer;
	ofstream out(filename.c_str(),fstream::app);
  	//out.open(filename.c_str(),fstream::app);
	if (timed) {
		timer.reset();
		while(timer.elapsedWallclock() < duration) {
			MPI::COMM_WORLD.Bcast(&stepOK, 1, MPI::INT, MASTER);
			firingPerStep = 0;
			for (unsigned worker = 1; worker < workers; ++worker) {
				MPI::COMM_WORLD.Recv(&stepDONE, 1, MPI::INT, worker, SIM_STEP, status);
				firingPerStep += stepDONE;
			}
			//out << timer.elapsedSimulation() << " " << firingPerStep << endl;
			timer.step();
		}
		runtime = timer.elapsedWallclock();
	} else {
		timer.reset();
		while(timer.elapsedSimulation() < duration) {
			MPI::COMM_WORLD.Bcast(&stepOK, 1, MPI::INT, MASTER);
			firingPerStep = 0;
			for (unsigned worker = 1; worker < workers; ++worker) {
				MPI::COMM_WORLD.Recv(&stepDONE, 1, MPI::INT, worker, SIM_STEP, status);
				firingPerStep += stepDONE;
			}
			//out << timer.elapsedSimulation() << " " << firingPerStep << endl;
			timer.step();
		}
		runtime = timer.elapsedWallclock();
	}
	//out.close();
	stepOK = 1;
	MPI::COMM_WORLD.Bcast(&stepOK, 1, MPI::INT, MASTER);
	unsigned totalfirings=0,totalspikes=0,buf;
	for (unsigned worker = 1; worker < workers; ++worker) {
		MPI::COMM_WORLD.Recv(&buf, 1, MPI::INT, worker, FIRINGS, status);
		totalfirings+= buf;
		MPI::COMM_WORLD.Recv(&buf, 1, MPI::INT, worker, SPIKES, status);
		totalspikes+= buf;
	}
	if (out.is_open()) {
		out << neuronCount << " " << runtime << endl;
		out.close();
	}
	if(verbose) {
		if (timed) cout << "Total # of steps succeded: " << timer.elapsedSimulation() << endl;
		else cout << "Runnning time of the simulation: " << runtime << endl;
		cout << "Total # of inter-nodal firings: " << totalfirings << endl;
		cout << "Total # of spikes delivered: " << totalspikes << endl;
	}

}
	}
}
