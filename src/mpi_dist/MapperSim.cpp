#include "MapperSim.hpp"
#include "parsing.hpp"
#include <vector>
#include <nemo/util.h>
#include <nemo/exception.hpp>
#include <math.h>
#define THRESHOLD 10
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
	this->auxMap.resize(neurons);
	vector <unsigned> partition;
	partition.resize(neurons);
	for (unsigned i = 0; i < neurons; ++i) partition[i] = i;
	allocateNeurons(net, partition, workers, 0, 0);
	auxMap.clear();
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

void
MapperSim::allocateNeurons(const nemo::Network& network, vector<unsigned>& partition, unsigned clusters, unsigned starting_cluster, int partcount)
{
	if (clusters == 1) {
		neuronMap[starting_cluster].resize(partition.size());
		for (unsigned i = 0; i < partition.size(); ++i) {
			neuronMap[starting_cluster][i] = partition[i];
			backMap[partition[i]] = i;
		}
		return;
	}
	unsigned ncount = partition.size();
	unsigned** matrix = new unsigned*[ncount];
	float** q_matrix = new float*[ncount];
	unsigned* degrees = new unsigned [ncount];
	float* eigenvector = new float [ncount];
	float* tmp = new float [ncount];
	unsigned i,j, edges = 0, step = 0;
	float norm, norm_sq;
	for (i = 0; i < ncount; ++i) {
		auxMap[partition[i]] = partcount;
		backMap[partition[i]] = i;
		matrix [i] = new unsigned[ncount];
		q_matrix [i] = new float[ncount];
		degrees[i] = 0;
		eigenvector[i] = 1;
		for (j = 0; j < ncount; ++j) matrix[i][j] = 0;
	}
	
	nemo::network::NetworkImpl net = *network.m_impl;
	for(i = 0; i < ncount; ++i) {
		vector<synapse_id> synapses = net.getSynapsesFrom(partition[i]);
		for (j = 0; j < synapses.size(); ++j) {
			unsigned globtarget = net.getSynapseTarget(synapses[j]);
			if (auxMap[globtarget] == partcount) {
				unsigned target = backMap[globtarget];
				matrix[i][target]++;
				matrix[target][i]++;
				degrees[i]++;
				degrees[target]++;
				edges++;
			}
		}
	}
	
	for (i = 0; i < ncount; ++i) {
		for (j = 0; j < ncount; ++j) {
			if (i != j) q_matrix[i][j] = (matrix[i][j] - (float)(degrees[i]*degrees[j])/(2*edges));
			else q_matrix[i][j] = 15;
		}
	}
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
	
	delete [] degrees;
	delete [] tmp;
	for (i = 0; i < ncount; ++i) {
		delete [] matrix[i];
		delete [] q_matrix[i];
	}
	delete [] matrix;
	delete [] q_matrix;
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
	partition.clear();
	allocateNeurons(network, partition1, clusters/2, starting_cluster, partcount*2-1);
	allocateNeurons(network, partition2, clusters/2 + clusters % 2, starting_cluster + clusters/2, partcount*2-2);
}


	} // namespace mpi_dist
} // namespace nemo
