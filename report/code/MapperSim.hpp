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
	/* Set variables */
	unsigned workers;
	unsigned neurons;

	/* Two maps:
		neuronMap - for local-to-global mapping
		backMap - for global-to-local mapping
	*/
	std::vector <unsigned>* neuronMap;
	std::vector <unsigned> backMap;
public:
	/* Different constructors for Master and workers */
	MapperSim(unsigned workerCount);
	MapperSim(const nemo::Network& net, unsigned workerCount);
	void set(const std::vector<std::string>& mapperData, unsigned neuronCount);
	
	/* Returns MPI ID of the owner */
	unsigned rankOf(unsigned neuron) const;
	unsigned neuronCount() const;
	unsigned workerCount() const;
	std::vector <unsigned> retrieveNeurons(unsigned rank);

	/* Functions for local-to-global and global-to-local mapping of neuron ids */
	unsigned mapLocal(unsigned gidx);
	unsigned mapGlobal(unsigned lidx, unsigned rank);

	/* Cluster mapping functions - called once on Master node */
	/* Uniform cluster mapping function - strict division neuronsPerWorker = neurons/worker, last worker gets offset */
	void allocateNeuronsUniform(const nemo::Network& net);

	/* Cluster mapping function - employs Newman's algorithm through calculating Q (modularity matrix) and the dominant eigenvector for it */
	void allocateNeurons(const nemo::Network& net, unsigned ncount);
};
	} // namespace mpi_dist
} // namespace nemo
#endif
