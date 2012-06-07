#include <vector>
#include <deque>
#include <set>
#include <nemo/Network.hpp>

class WorkerSimulation {

	public:

		WorkerSimulation::WorkerSimulation();
		~WorkerSimulation();

	private:
		
		Configuration configuration;
		WorkerSimulation::receiveConfiguration();
		WorkerSimulation::receiveNeurons();
		WorkerSimulation::receiveSynapses();
		/* Two queues: incoming and outgoing spikes */

}
