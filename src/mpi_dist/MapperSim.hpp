#ifndef MAPPER_SIM_HPP
#define MAPPER_SIM_HPP
/* This is a mapper class that would define the clusterization of the network
 */

// The implementation of the cluster mapping algorithm is going to be here.
// Then depending on the worker id, the particular set of neurons could be called.

#include <nemo/internal_types.h>

using namespace std;

namespace nemo {
	namespace mpi_dist {

class MapperSim
{
public:
	MapperSim(unsigned neuronCount, unsigned workerCount);
	unsigned rankOf(unsigned neuron) const;
	unsigned neuronCount() const;
	unsigned workerCount() const;
	unsigned workerSize() const;

private:
	unsigned workers;
	unsigned neurons;
	unsigned m_workerSize;
};
	}
}
#endif