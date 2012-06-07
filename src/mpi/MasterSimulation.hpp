#include "mpi.h"
#include "Mapper.hpp"

namespace nemo {
	namespace mpi {

class MasterSimulation {

	public :
		
		MasterSimulation(const Network&, const Configuration&);
		~MasterSimulation();

	private :

		MPI::Status status;
		Mapper mapper;
		unsigned workers;
		void distributeConfiguration();
		void distributeNeurons();
		void distributeSynapses();
		float* getNeuron(unsigned idx, const Network&);

};

	}
}
