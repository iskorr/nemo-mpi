#include "mpi.h"
#include "Mapper.hpp"

namespace nemo {
	namespace mpi {

class MasterSimulation {

	public :
		
		Master(const Network&, const Configuration&);
		~Master();

	private :

		MPI::Status status;
		Mapper mapper;
		void distributeNeurons();
		void distributeSynapses();

};

	}
}
