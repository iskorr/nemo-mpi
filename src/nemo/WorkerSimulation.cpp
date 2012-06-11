#include "mpi.h"
#include "WorkerSimulation.hpp"
#include "mpi_tags.hpp"
#include "parsing.hpp"

#include <vector>
#include <string>
#include <iostream>
#include <boost/scoped_ptr.hpp>

#include <nemo/Simulation.hpp>
#include <nemo/SimulationBackend.hpp>
#include <nemo.hpp>
using namespace std;

WorkerSimulation::WorkerSimulation(unsigned rank, unsigned workers) :
						rank(rank), workers(workers)
{
	nemo::Network* net = new nemo::Network();
	nemo::Configuration conf;
	receiveConfiguration(conf);
	receiveNeurons(net);
	receiveSynapses(net);
	reply = 1;
	MPI::COMM_WORLD.Send(&reply, 1, MPI::INT, MASTER, DISTRIBUTION_COMPLETE);
	nemo::Simulation* sim = nemo::simulation(*net, conf);
	MPI_Barrier(MPI_COMM_WORLD);
	runSimulation(sim);
}

WorkerSimulation::~WorkerSimulation()
{
	reply = 0;
	MPI::COMM_WORLD.Send(&reply, 1, MPI::INT, MASTER, DISTRIBUTION_COMPLETE);
}



void
WorkerSimulation::runSimulation(nemo::Simulation* sim)
{
	unsigned stepOK, stepDONE=1;
	MPI::COMM_WORLD.Bcast(&stepOK, 1, MPI::INT, MASTER);
	while(stepOK == 0) {
		vector <pair<unsigned,float> > stim (stim_template);
		enqueueIncomingSpikes(sim, stim);
		distributeOutgoingSpikes(sim->step(stim));
		MPI::COMM_WORLD.Send(&stepDONE, 1, MPI::INT, MASTER, SIM_STEP);
		MPI::COMM_WORLD.Bcast(&stepOK, 1, MPI::INT, MASTER);
	}

}


void
WorkerSimulation::enqueueIncomingSpikes(nemo::Simulation* sim, vector <pair<unsigned,float> >& stimulus)
{
	while(!incoming.empty()) {
		vector < vector <float> > synData = getSynapseData(incoming.front());
		for (unsigned i = 1; i < synData.size();++i) stimulus[synData[i][0]].second += synData[i][2];
		incoming.pop_front();
	}
}

void
WorkerSimulation::distributeOutgoingSpikes(const vector <unsigned>& output)
{
	unsigned msg,count = 0;
	int buf;
	for(unsigned id = 0; id < output.size(); ++id) {
		for (unsigned target = 0; target < outgoingSynapses[id].size(); ++target) {
			msg = mapGlobal(output[id]);
			cout << "Sending " << msg << " to " << outgoingSynapses[id][target] << endl;
			send_request = MPI::COMM_WORLD.Isend(&msg, 1, MPI::INT, outgoingSynapses[id][target], COMMUNICATION_TAG);
		}
		send_request.Wait(status);
	}
	msg = -1;
	for (unsigned worker = 1; worker < workers; ++worker) MPI::COMM_WORLD.Isend(&msg, 1, MPI::INT, worker, COMMUNICATION_TAG);
	unsigned worker = 1;
	while(count < workers-2) {
		if (rank == worker && worker < workers-1) worker++;
		else if (worker == workers) break;
		MPI::COMM_WORLD.Recv(&buf, 1, MPI::INT, worker, COMMUNICATION_TAG, status);
		if (buf > -1) incoming.push_back(buf);
		else count++;worker++;
	}
}



//-------------------------------------------------------------------------------------------------------------
/* The initialisation of the network */
void
WorkerSimulation::receiveConfiguration(nemo::Configuration& configuration)
{
	unsigned confLength;
	MPI::COMM_WORLD.Recv(&confLength, 1, MPI::INT, MASTER, CONFIGURATION_LENGTH_TAG, status);	
	char confdata [confLength];
	MPI::COMM_WORLD.Recv(&confdata, confLength, MPI::CHAR, MASTER, CONFIGURATION_DATA_TAG, status);
	decodeConfiguration(configuration, confdata);
}

void
WorkerSimulation::receiveNeurons(nemo::Network* net)
{
	unsigned neuronLength;
	MPI::COMM_WORLD.Recv(&neuronCount, 1, MPI::INT, MASTER, NEURON_COUNT_TAG, status);
	for (unsigned nidx = 0; nidx < neuronCount; ++nidx) {
		MPI::COMM_WORLD.Recv(&neuronLength, 1, MPI::INT, MASTER, NEURON_LENGTH_TAG, status);
		char neuronData [neuronLength];
		MPI::COMM_WORLD.Recv(&neuronData, neuronLength, MPI::CHAR, MASTER, NEURON_DATA_TAG, status);
		float* params = decodeNeuron(neuronData);
		net->addNeuron((unsigned)params[7], params[0], params[1], params[2], params[3], params[4], params[5], params[6]);
	}
	for (unsigned nidx = 0; nidx < neuronCount; ++nidx) {
		pair <unsigned, float> p (nidx,0);
		stim_template.push_back(p);	
	}
}

void
WorkerSimulation::receiveSynapses(nemo::Network* net)
{
	for (unsigned i = 0; i < neuronCount;++i) {
		outgoingSynapses.push_back(vector <unsigned>());
	}
	unsigned ok,synLength;
	while(true) {
		MPI::COMM_WORLD.Recv(&ok,1,MPI::INT,MASTER,SYNAPSE_END_TAG,status);
		if (ok == 1) break;
		MPI::COMM_WORLD.Recv(&synLength, 1, MPI::INT, MASTER, SYNAPSE_LENGTH_TAG, status);
		char synData [synLength];
		MPI::COMM_WORLD.Recv(&synData, synLength, MPI::CHAR, MASTER, SYNAPSE_DATA_TAG, status);
		assignSynapse(net, decode(synData, ","));
	}
}

void
WorkerSimulation::assignSynapse(nemo::Network* net, vector<string> synParams) {
	float* result = new float [synParams.size()];
	for (unsigned i = 0; i < synParams.size()-1; ++i) result[i] = ::atof(synParams[i].c_str());
	if ((int) result[1] > 0 || ((int) result[1] == 0 && rank == 1)) {	
		if ((int) result[0] > 0) 
			net->addSynapse(mapLocal((unsigned) result[0]), mapLocal((unsigned) result[1]), (unsigned) result[2], result[3], (unsigned char)atoi(synParams[4].c_str()));
		else if ((int) result[0] == 0 && rank == 1)
			net->addSynapse((unsigned) result[0], mapLocal((unsigned) result[1]), (unsigned) result[2], result[3], (unsigned char)atoi(synParams[4].c_str()));
		else
			addIncomingSynapse(((int) result[0])*-1, mapLocal((unsigned) result[1]), (unsigned) result[2], result[3]);
	} else {
		addOutgoingSynapse(mapLocal((unsigned) result[0]), (int) result[1]*-1);
	}
}

void
WorkerSimulation::addIncomingSynapse(unsigned source, unsigned target, unsigned delay, float weight)
{
	unsigned i = 0;
	for (; i < incomingSynapses.size();++i) {
		if (incomingSynapses[i][0][0] == source) break;
	}
	if (i < incomingSynapses.size()) {
		float syn[] = {(float)target,weight};
		std::vector<float> synv(syn, syn + sizeof(syn) / sizeof(float));
		incomingSynapses[i].push_back(synv);
	} else {
		float src_f[] = {float(source), 0};
		float trg_f[] = {float(target), delay, weight};
		std::vector<float> src(src_f, src_f + sizeof(src_f) / sizeof(float));
		std::vector<float> trg(trg_f, trg_f + sizeof(trg_f) / sizeof(float));
		std::vector<std::vector<float> > n_src;
		n_src.push_back(src);
		n_src.push_back(trg);
		incomingSynapses.push_back(n_src);
	}
}

void
WorkerSimulation::addOutgoingSynapse(unsigned source, unsigned target)
{
	unsigned i = 0, targetRank = 1 + target/neuronCount;
	if (outgoingSynapses[source].size() > 0) {
		for (; i < outgoingSynapses[source].size();++i) {
			if (outgoingSynapses[source][i] == targetRank) break;
		}
		if (i > outgoingSynapses[source].size()-1) outgoingSynapses[source].push_back(targetRank);
	} else outgoingSynapses[source].push_back(targetRank);
}

unsigned
WorkerSimulation::mapLocal(unsigned gidx) {
	return gidx - (rank-1)*neuronCount;
}

unsigned
WorkerSimulation::mapGlobal(unsigned lidx) {
	return lidx + (rank-1)*neuronCount;
}

vector <vector <float> >
WorkerSimulation::getSynapseData(unsigned source)
{
	unsigned i = 0;
	while ((unsigned) incomingSynapses[i][0][0] != source) i++;
	return incomingSynapses[i];
}

/*
bool
WorkerSimulation::commTest()
{
	unsigned msgcount = 0,count = 0, worker = 1;
	for (; worker < workers; ++worker) {
		if (worker != rank) {
			unsigned i = 1;
			for (; i < 100001; ++i) {
				send_request = MPI::COMM_WORLD.Isend(&i, 1, MPI::INT, worker, COMMUNICATION_TAG);
				send_request.Wait(status);
			}
			i = 0;
			send_request = MPI::COMM_WORLD.Isend(&i, 1, MPI::INT, worker, COMMUNICATION_TAG);
		}
	}
	worker = 1;
	unsigned buf;
	while(count < workers-2) {
		if (worker == rank) worker++;
		MPI::COMM_WORLD.Recv(&buf, 1, MPI::INT, worker, COMMUNICATION_TAG, status);
		if (buf > 0) msgcount++;
		else {
			count++;worker++;
		}
	}
	MPI_Wait(send_request,status);
	cout << "Total of " << msgcount << " messages were received" << endl;
	return true;
}*/
