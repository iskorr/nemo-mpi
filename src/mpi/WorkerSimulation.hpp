#include <vector>
#include <deque>
#include <set>
#include <nemo/Network.hpp>

namespace nemo {

	class ConfigurationImpl;
	class Configuration;

	namespace mpi {

class BlockingWorker {

	public :

		BlockingWorker (MPI_Comm world);

	private :

		void addDummyNeuron(nemo::Network* net, unsigned id);
		nemo::Network* construct(unsigned ncount, unsigned scount);

};

	}
}
