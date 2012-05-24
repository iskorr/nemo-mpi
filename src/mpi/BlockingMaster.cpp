#include "mpi.h"
#include <iostream>

using namespace std;

namespace nemo {
	namespace mpi {

BlockingMaster::BlockingMaster(unsigned neuronCount, unsigned nodes, MPI_Comm comm)
{
	// Setting up the network
	distributeNeurons();
	distributeSynapses();
}

void BlockingMaster::distributeNeurons()
{
	// Sending phase
	int neuronsPerNode = neuronCount/nodeCount;

	for (int worker = 1; worker <= nodeCount; worker++) {
		MPI_Send(&neuronsPerNode, 1, MPI_INT, worker,1, world);
	}

	cout << "The neuron numbers sent to " << nodeCount << " workers" << endl;

	// Validation phase
	int reply, worker = 1;
	MPI_Status stat;

	for (; worker <= nodeCount; worker++) {
		MPI_Recv(&reply, 1, MPI_INT, worker,2, world, &stat);
		if(reply == 1) continue;
		else break;
	}

	cout << "Neurons received by " << worker << " workers" << endl;
}

void BlockingMaster::distributeSynapses()
{

}


	}
}
