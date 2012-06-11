#include "MapperSim.hpp"
#include <nemo/util.h>
#include <nemo/exception.hpp>

MapperSim::MapperSim(unsigned neuronCount, unsigned workerCount) : 
					workers(workerCount),
					neurons(neuronCount)
{
	if (workers == 0) {
		throw nemo::exception(NEMO_MPI_ERROR, "No worker nodes");
	}
	m_workerSize = DIV_CEIL(neurons,workers);
}

unsigned
MapperSim::rankOf(unsigned neuron) const
{
	return 1 + neuron / m_workerSize;
}

unsigned
MapperSim::workerSize() const
{
	return m_workerSize;
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
