/* This is a mapper class that would define the clusterization of the network
 */
#ifndef MAPPER_SIM_HPP
#define MAPPER_SIM_HPP
#include <nemo/internal_types.h>
#include <nemo/Network.hpp>
#include <string>

namespace nemo {
	namespace mpi_dist {

class MapperSim
{
private:
	unsigned workers;
	unsigned neurons;
	std::vector <unsigned>* neuronMap;
	std::vector <unsigned> backMap;
public:
	MapperSim(unsigned workerCount);
	MapperSim(const nemo::Network& net, unsigned workerCount);
	void set(const std::vector<std::string>& mapperData, unsigned neuronCount);
	unsigned rankOf(unsigned neuron) const;
	unsigned neuronCount() const;
	unsigned workerCount() const;
	std::vector <unsigned> retrieveNeurons(unsigned rank);
	unsigned mapLocal(unsigned gidx);
	unsigned mapGlobal(unsigned lidx, unsigned rank);
	/* Cluster mapping */
	void allocateNeurons(const nemo::Network& net);

};
	}
}
#endif
