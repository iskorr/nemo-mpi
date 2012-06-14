#include "MapperSim.hpp"
#include "parsing.hpp"
#include <vector>
#include <nemo/util.h>
#include <nemo/exception.hpp>
#include <math.h>
#define THRESHOLD 100
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
	//allocateNeurons(net, neurons);
	allocateNeuronsUniform(net);
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
MapperSim::allocateNeurons(const nemo::Network& network, unsigned ncount)
{
	unsigned** matrix = new unsigned*[ncount];
	float** q_matrix = new float*[ncount];
	unsigned* degrees = new unsigned [ncount];
	float* eigenvector = new float [ncount];
	float* tmp = new float [ncount];
	unsigned i,j, edges = 0, step = 0;
	float norm, norm_sq;
	for (i = 0; i < ncount; ++i) {
		matrix [i]=new unsigned[ncount];
		q_matrix [i]=new float[ncount];
		degrees[i] = 0;
		eigenvector[i] = 1;
		for (j = 0; j < ncount; ++j) matrix[i][j] = 0;
	}

	nemo::network::NetworkImpl net = *network.m_impl;
	for(nemo::network::synapse_iterator s = net.synapse_begin(); s != net.synapse_end(); ++s) {
		matrix[s->source][s->target()]++;
		matrix[s->target()][s->source]++;
		degrees[s->source]++;
		degrees[s->target()]++;
		edges++;
	}
	for (i = 0; i < ncount; ++i) {
		for (j = 0; j < ncount; ++j) {
			q_matrix[i][j] = q_matrix[i][j] - (float)(degrees[i]*degrees[j])/(2*edges);
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
	/* Comparison */
/*	int sumM = 0, sum = 0;
	for (i = 0; i < ncount; ++i) {
		if (eigenvector[i] >= 0) {
			for (j = 0; j < ncount; ++j) {
				if (eigenvector[j] < 0 && i != j) sum += matrix[i][j];
			}
		}
		if (i < ncount/2) {
			for (unsigned j = ncount/2; j < ncount; ++j) {
				sumM += matrix[i][j];
			}
		}
	}
	cout << "Improvement is " << sum - sumM << " Total # of edges is " << edges << endl;*/
}


	}// namespace mpi_dist
}// namespace nemo
