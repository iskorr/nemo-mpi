#include "WorkerSimulation.hpp"
#include "mpi_tags.hpp"
#include "parsing.hpp"
#include <vector>
#include <string>
#include <boost/scoped_ptr.hpp>
#include <nemo/Simulation.hpp>
#include <nemo/SimulationBackend.hpp>
#include <nemo.hpp>
using namespace std;

namespace nemo {
	namespace mpi_dist {

WorkerSimulation::WorkerSimulation(unsigned rank, unsigned workerCount) :
						mapper(workerCount-1), rank(rank),  workers(workerCount)
{
	outfirings = 0, spikes = 0, spikesPerStep = 0;
	nemo::Network* net = new nemo::Network();
	nemo::Configuration conf;
	receiveMapper();
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
	unsigned stepOK;
	MPI::COMM_WORLD.Bcast(&stepOK, 1, MPI::INT, MASTER);
	while(stepOK == 0) {
		vector <pair<unsigned,float> > stim (stim_template);
		enqueueIncomingSpikes(sim, stim);
		distributeOutgoingSpikes(sim->step(stim));
		MPI::COMM_WORLD.Send(&spikesPerStep, 1, MPI::INT, MASTER, SIM_STEP);
		spikesPerStep = 0;
		MPI::COMM_WORLD.Bcast(&stepOK, 1, MPI::INT, MASTER);
	}
	MPI::COMM_WORLD.Send(&outfirings, 1, MPI::INT, MASTER, FIRINGS);
	MPI::COMM_WORLD.Send(&spikes, 1, MPI::INT, MASTER, SPIKES);
}


void
WorkerSimulation::enqueueIncomingSpikes(nemo::Simulation* sim, vector <pair<unsigned,float> >& stimulus)
{
	vector <inc_syn> synData;
	while(!incoming.empty()) {
		synData = getSynapseData(incoming.front());
		for (unsigned i = 0; i < synData.size();++i) {
			pair<unsigned,float> entry(synData[i].target,synData[i].weight);
			delay_queue[synData[i].delay-1].push_back(entry);
		}
		incoming.pop_front();
	}
	if (!delay_queue.empty()) {
		deque <pair<unsigned, float> > slot = delay_queue.front();
		while (!slot.empty()) {
			stimulus[slot.front().first].second += slot.front().second;
			spikes++;
			spikesPerStep++;
			slot.pop_front();
		}
		delay_queue.pop_front();
	}
	delay_queue.resize(32);
}

void
WorkerSimulation::distributeOutgoingSpikes(const vector <unsigned>& output)
{
	unsigned msg,count = 0;
	int buf;
	for(unsigned id = 0; id < output.size(); ++id) {
		unsigned fired = output[id];
		for (unsigned target = 0; target < outgoingSynapses[fired].size(); ++target) {
			msg = mapper.mapGlobal(fired,rank);
			send_request = MPI::COMM_WORLD.Isend(&msg, 1, MPI::INT, outgoingSynapses[fired][target], COMMUNICATION_TAG);
		}
		outfirings++;
		send_request.Wait(status);
	}
	msg = -1;
	for (unsigned worker = 1; worker < workers; ++worker) MPI::COMM_WORLD.Isend(&msg, 1, MPI::INT, worker, COMMUNICATION_TAG);
	unsigned worker = 1;
	while(count < workers-2) {
		if (rank == worker && worker < workers-1) worker++;
		else if (worker == workers) break;
		MPI::COMM_WORLD.Recv(&buf, 1, MPI::INT, worker, COMMUNICATION_TAG, status);
		if (buf > -1) {
			incoming.push_back(buf);
		} else {
			count++;
			worker++;
		}
	}
}



//-------------------------------------------------------------------------------------------------------------
/* The initialisation of the network */

void
WorkerSimulation::receiveMapper()
{
	unsigned mapLength,totalNeurons;
	MPI::COMM_WORLD.Recv(&totalNeurons, 1, MPI::INT, MASTER, TOTAL_NEURONS_TAG, status);
	vector <string> mapSet;
	mapSet.resize(workers-1);
	for (unsigned worker = 0; worker < workers-1; ++worker) {
		MPI::COMM_WORLD.Recv(&mapLength, 1, MPI::INT, MASTER, MAPPER_LENGTH_TAG, status);
		char mapdata [mapLength];
		MPI::COMM_WORLD.Recv(&mapdata, mapLength, MPI::CHAR, MASTER, MAPPER_DATA_TAG, status);
		mapSet[worker] = string(mapdata);
	}
	mapper.set(mapSet,totalNeurons);
}

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
		net->addNeuron(mapper.mapLocal((unsigned)params[7]), params[0], params[1], params[2], params[3], params[4], params[5], params[6]);
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
	if ((int) result[1] >= 0) {	
		if ((int) result[0] >= 0) 
			net->addSynapse(mapper.mapLocal((unsigned) result[0]), mapper.mapLocal((unsigned) result[1]), (unsigned) result[2], result[3], (unsigned char)atoi(synParams[4].c_str()));
		else {
			addIncomingSynapse((int)-(result[0]+1) , mapper.mapLocal((unsigned) result[1]), (unsigned) result[2], result[3]);
		}
	} else {
		addOutgoingSynapse(mapper.mapLocal((unsigned) result[0]), (int)-(result[1]+1));
	}
}

void
WorkerSimulation::addIncomingSynapse(unsigned source, unsigned target, unsigned delay, float weight)
{
	unsigned i = 0;
	bool found = false;
	inc_syn entry;
	entry.target = target;
	entry.delay = delay;
	entry.weight = weight;
	for (; i < incomingSynapses.size();++i) {
		if (incomingSynapses[i].first == source) {
			incomingSynapses[i].second.push_back(entry);
			found = true;
			break;
		}
	}
	if (!found) {
		vector <inc_syn> vec;
		vec.push_back(entry);
		pair<unsigned,vector<inc_syn> > src_entry (source, vec);
		incomingSynapses.push_back(src_entry);
	}
}

void
WorkerSimulation::addOutgoingSynapse(unsigned source, unsigned target)
{
	unsigned targetRank = mapper.rankOf(target);
	bool found = false;
	for (unsigned i = 0; i < outgoingSynapses[source].size();++i) {
		if (outgoingSynapses[source][i] == targetRank) {
			found = true;
		}
	}
	if (!found) outgoingSynapses[source].push_back(targetRank);
}

vector <inc_syn>
WorkerSimulation::getSynapseData(unsigned source)
{
	unsigned i = 0;
	for (;i < incomingSynapses.size(); ++i) {
		if (incomingSynapses[i].first == source) return incomingSynapses[i].second;
	}
	return vector <inc_syn>();
}

	}
}
