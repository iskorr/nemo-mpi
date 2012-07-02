#include "MapperSim.hpp"
#include "parsing.hpp"
#include <vector>
#include <iostream>
#include <nemo/util.h>
#include <nemo/exception.hpp>
#include <math.h>
#define THRESHOLD 10
#define MEM_MAX 51000
#define STRENGTH 15
using namespace std;

namespace nemo {
	namespace mpi_dist {

MapperSim::MapperSim(unsigned workerCount) : workers(workerCount)
{
	if (workers == 0) throw nemo::exception(NEMO_MPI_ERROR, "No worker nodes");
	this->neuronMap = new vector<unsigned> [workers];
}


MapperSim::MapperSim(const nemo::Network& net, unsigned workerCount) : 
					workers(workerCount), neurons(net.neuronCount())
{
	if (workers == 0) throw nemo::exception(NEMO_MPI_ERROR, "No worker nodes");
	this->neuronMap = new vector<unsigned> [workers];
	this->backMap.resize(neurons);
	vector <unsigned> partition;
	partition.resize(neurons);
	for (unsigned i = 0; i < neurons; ++i) partition[i] = i;
	if (neurons <= MEM_MAX) {
		this->auxMap.resize(neurons);
		allocateNeurons(net, partition, workers, 0, 0);
		auxMap.clear();
	} else {
		allocateNeuronsUniform(net);	
	}
}

void
MapperSim::set(const vector<string>& mapData, unsigned neuronCount)
{
	neurons = neuronCount;
	this->backMap.resize(neurons);
	unsigned neuron;
	for (unsigned worker = 0; worker < mapData.size(); ++worker) {
		vector<string> neuronSet = decode(mapData[worker],",");
		for (unsigned j = 0; j < neuronSet.size(); ++j) {
			neuron = atoi(neuronSet[j].c_str());
			neuronMap[worker].push_back(neuron);
			backMap[neuron] = j;
		}
	}
}

unsigned
MapperSim::rankOf(unsigned neuron) const
{
	unsigned i = 0, lidx = backMap[neuron];
	for (; i < workers; ++i) {
		if (neuronMap[i][lidx] == neuron) break;
	}
	return i + 1;
}


unsigned
MapperSim::neuronCount() const
{
	return neurons;
}

unsigned
MapperSim::workerCount() const
{
	return workers;
}

vector <unsigned>
MapperSim::retrieveNeurons(unsigned rank)
{
	return neuronMap[rank-1];
}

unsigned
MapperSim::mapLocal (unsigned gidx)
{
	return backMap[gidx];
}

unsigned
MapperSim::mapGlobal (unsigned lidx, unsigned rank)
{
	return neuronMap[rank-1][lidx];
}

void
MapperSim::allocateNeuronsUniform(const nemo::Network& net)
{
	unsigned buf, neuronsPerWorker = neurons / workers;
	unsigned lastNeurons = neuronsPerWorker + (neurons % workers);
	for (unsigned worker = 0; worker < workers-1; ++worker) {
		neuronMap[worker].resize(neuronsPerWorker);
		for (unsigned i = 0; i < neuronsPerWorker; ++i) {
			buf = i + neuronsPerWorker*worker;
			neuronMap[worker][i] = buf;
			backMap[buf] = i;
		}
	}
	unsigned offset = neuronsPerWorker*(workers-1);
	neuronMap[workers-1].resize(lastNeurons);
	for (unsigned j = 0; j < lastNeurons; ++j) {
		neuronMap[workers-1][j] = j + offset;
		backMap[j+offset] = j;
	}
}

// Mapping Algorithm implementation
void
MapperSim::allocateNeurons(const nemo::Network& network, vector<unsigned>& partition, unsigned clusters, unsigned starting_cluster, int partcount)
{
	// Recursive version - this is the base case
	if (clusters == 1) {
		neuronMap[starting_cluster].resize(partition.size());
		for (unsigned i = 0; i < partition.size(); ++i) {
			neuronMap[starting_cluster][i] = partition[i];
			backMap[partition[i]] = i;
		}
		return;
	}

	// Variables instantiation - q_matrix is the most important (used for both adjacency and modularity)
	unsigned ncount = partition.size();
	float** q_matrix = new float*[ncount];
	unsigned* degrees = new unsigned [ncount];
	float* eigenvector = new float [ncount];
	float* tmp = new float [ncount];
	unsigned i,j, edges = 0, step = 0;
	float norm, norm_sq;
	for (i = 0; i < ncount; ++i) {
		auxMap[partition[i]] = partcount;
		backMap[partition[i]] = i;
		q_matrix [i] = new float[ncount];
		degrees[i] = 0;
		eigenvector[i] = 1;
		for (j = 0; j < ncount; ++j) q_matrix[i][j] = 0;
	}

	// One part of original NeMo that has been modified - accessible NetworkImpl within Network class
	nemo::network::NetworkImpl net = *network.m_impl;

	// Step 1 = Populating Adjacency Matrix
	for(i = 0; i < ncount; ++i) {
		vector<synapse_id> synapses = net.getSynapsesFrom(partition[i]);
		for (j = 0; j < synapses.size(); ++j) {
			unsigned globtarget = net.getSynapseTarget(synapses[j]);
			if (auxMap[globtarget] == partcount) {
				unsigned target = backMap[globtarget];
				q_matrix[i][target]++;
				q_matrix[target][i]++;
				degrees[i]++;
				degrees[target]++;
				edges++;
			}
		}
	}

	// Step 2 = Population of Modularity Matrix using previously collected values for adjacency and degrees per neuron
	for (i = 0; i < ncount; ++i) {
		for (j = 0; j < ncount; ++j) {
			if (i != j) q_matrix[i][j] = (q_matrix[i][j] - (float)(degrees[i]*degrees[j])/(2*edges));
			else q_matrix[i][j] = STRENGTH;
		}
	}

	// Step 3 = Finding leading eigenvector (Von Moses algorithm, power-iteration)
	while(step < THRESHOLD) {
		norm_sq=0;
		for (i = 0; i < ncount; ++i) {
	        	tmp[i] = 0;
        		for (j = 0; j < ncount; ++j) tmp[i] += q_matrix[i][j] * eigenvector[j];
			norm_sq += tmp[i]*tmp[i];
    		}
    		norm = sqrt(norm_sq);
		for (i = 0; i < ncount; ++i) eigenvector[i] = tmp[i]/norm;
		step++;
	}

	// Cleaning up
	delete [] degrees;
	delete [] tmp;
	for (i = 0; i < ncount; ++i) {
		delete [] q_matrix[i];
	}
	delete [] q_matrix;

	// Step 4 = Partitioning the graph
	vector<unsigned> partition1;
	vector<unsigned> partition2;
	for (i = 0; i < ncount; ++i) {
		if (eigenvector[i] > 0) {
			partition1.push_back(partition[i]);
		} else {
			partition2.push_back(partition[i]);
		}	
	}
	delete [] eigenvector;

	// The check for empty graphs - if one of partitions is empty - do a uniform split
	if (partition1.size() == 0 || partition2.size() == 0) {
		partition1.clear();
		partition2.clear();
		for (i = 0; i < ncount/2; ++i) partition1.push_back(partition[i]);
		for (i = ncount/2; i < ncount; ++i) partition2.push_back(partition[i]);
	}
	partition.clear();

	// Step 5 = Recursive call for the next partition - on both sides
	allocateNeurons(network, partition1, clusters/2, starting_cluster, partcount*2-1);
	allocateNeurons(network, partition2, clusters/2 + clusters % 2, starting_cluster + clusters/2, partcount*2-2);
}


	} // namespace mpi_dist
} // namespace nemo
