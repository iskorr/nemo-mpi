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

WorkerSimulation::WorkerSimulation(unsigned rank, unsigned workerCount) : mapper(workerCount-1), rank(rank), reply(0), workers(workerCount), fired(0), spikes(0), spikesPerStep(0), firedPerStep(0)
{
	sent = 0, received = 0;
	nemo::Network* net = new nemo::Network();
	nemo::Configuration conf;
	receiveMapper();
	receiveConfiguration(conf);
	receiveNeurons(net);
	receiveSynapses(net);
	MPI::COMM_WORLD.Send(&reply, 1, MPI::INT, MASTER, DISTRIBUTION_COMPLETE);
	nemo::Simulation* sim = nemo::simulation(*net, conf);
	MPI_Barrier(MPI_COMM_WORLD);
	runSimulation(sim);
}

WorkerSimulation::~WorkerSimulation()
{
	reply = 1;
	MPI::COMM_WORLD.Send(&reply, 1, MPI::INT, MASTER, DISTRIBUTION_COMPLETE);
}

void
WorkerSimulation::runSimulation(nemo::Simulation* sim)
{
	unsigned stepOK;
	while(true) {
		vector <pair<unsigned,float> > stim (stim_template);
		enqueueIncomingSpikes(sim, stim);
		MPI::COMM_WORLD.Bcast(&stepOK, 1, MPI::INT, MASTER);
		if (stepOK > 1) break;
		distributeOutgoingSpikes(sim->step(stim));
		MPI::COMM_WORLD.Send(&firedPerStep, 1, MPI::INT, MASTER, SIM_STEP);
		firedPerStep = 0, spikesPerStep = 0;
	}
	MPI::COMM_WORLD.Send(&spikes, 1, MPI::INT, MASTER, SPIKES);
}


//------------------------------------------------------------------------------------------------------------
/* Step functions */

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
		spikesPerStep = slot.size();
		spikes += spikesPerStep;
		while (!slot.empty()) {
			stimulus[slot.front().first].second += slot.front().second;
			slot.pop_front();
		}
		delay_queue.pop_front();
	}
	delay_queue.resize(32);
}

void
WorkerSimulation::distributeOutgoingSpikes(const vector <unsigned>& output)
{
	firedPerStep = output.size();
	fired += firedPerStep;
	int msg;
	for(unsigned id = 0; id < output.size(); ++id) {
		unsigned fired = output[id];
		for (unsigned target = 0; target < outgoingSynapses[fired].size(); ++target) {
			msg = mapper.mapGlobal(fired,rank);
			send_request = MPI::COMM_WORLD.Isend(&msg, 1, MPI::INT, outgoingSynapses[fired][target], COMMUNICATION_TAG);
			sent++;
		}
		send_request.Wait(status);
	}
	int buf, end = -1;
	for (unsigned worker = 1; worker < workers; ++worker) {
		if (rank != worker) MPI::COMM_WORLD.Isend(&end, 1, MPI::INT, worker, COMMUNICATION_TAG);
	}
	send_request.Wait(status);
	unsigned worker = 1, count = workers-2;
	while(count > 0) {
		if (rank == worker && worker < workers-1) worker++;
		else if (worker == workers) break;
		recv_request = MPI::COMM_WORLD.Irecv(&buf, 1, MPI::INT, worker, COMMUNICATION_TAG);
		if (!MPI::COMM_WORLD.Iprobe(worker, COMMUNICATION_TAG, status)) {
			MPI_Cancel(recv_request);
			count--;		
		}
		if (buf > -1) {
			incoming.push_back(buf);
			received++;
		} else {
			count--;
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
	MPI::COMM_WORLD.Recv(&neuronCount, 1, MPI::INT, MASTER, NEURON_COUNT_TAG, status);
	float params [NEURON_ARGS];
	for (unsigned nidx = 0; nidx < neuronCount; ++nidx) {
		MPI::COMM_WORLD.Recv(&params,NEURON_ARGS,MPI::FLOAT,MASTER,NEURON_DATA_TAG, status);
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
	float params[SYNAPSE_ARGS];
	while(true) {
		MPI::COMM_WORLD.Recv(&params, SYNAPSE_ARGS, MPI::FLOAT, MASTER, SYNAPSE_DATA_TAG,status);
		if (params[0] < 0) break;
		assignSynapse(net, params);
	}
}

void
WorkerSimulation::assignSynapse(nemo::Network* net, float* params) {
	unsigned source = (unsigned) params[0];
	unsigned target = (unsigned) params[1];
	unsigned sourceRank = mapper.rankOf(source);
	unsigned targetRank = mapper.rankOf(target);
	if (targetRank == rank) {	
		if (sourceRank == rank) 
			net->addSynapse(mapper.mapLocal(source), mapper.mapLocal(target), (unsigned) params[2], params[3], (unsigned char) params[4]);
		else {
			addIncomingSynapse(source , mapper.mapLocal(target), (unsigned) params[2], params[3]);
		}
	} else {
		addOutgoingSynapse(mapper.mapLocal(source), target);
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
