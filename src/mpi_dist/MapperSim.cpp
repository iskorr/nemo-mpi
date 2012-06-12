#include "MapperSim.hpp"
#include "parsing.hpp"
#include <vector>
#include <nemo/util.h>
#include <nemo/exception.hpp>
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
MapperSim::allocateNeurons(const nemo::Network& net)
{
	//unsigned matrix [neurons][neurons];
}

	}
}
