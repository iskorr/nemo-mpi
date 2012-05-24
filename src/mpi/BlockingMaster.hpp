#include "mpi.h"

namespace nemo {
	namespace mpi {

class BlockingMaster {

	public :
		
		BlockingMaster (unsigned neuronCount, unsigned nodes, MPI_Comm comm);

	private :

		MPI_Comm world;
		unsigned nodeCount;
		unsigned neuronCount;
		void distributeNeurons();
		void distributeSynapses();

};

	}
}
