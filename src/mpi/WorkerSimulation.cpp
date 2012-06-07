#define MASTER 0
#define CONFIGURATION_LENGTH_TAG 0
#define CONFIGURATION_DATA_TAG 1
#define NEURON_COUNT_TAG 2
#define NEURON_LENGTH_TAG 3
#define NEURON_DATA_TAG 4
#define SYNAPSE_LENGTH_TAG 5
#define SYNAPSE_DATA_TAG 6
#include <vector>
#include "parsing.hpp"

WorkerSimulation::WorkerSimulation()
{
	receiveConfiguration();
	receiveNeurons();
	receiveSynapses();
}

void
WorkerSimulation::receiveConfiguration()
{
	unsigned confLength;
	MPI::COMM_WORLD.Recv(&confLength, 1, MPI::INT, MASTER, CONFIGURATION_LENGTH_TAG, status);	
	char confdata [confLength];
	MPI::COMM_WORLD.Recv(&confdata, confLength, MPI::CHAR, MASTER, CONFIGURATION_DATA_TAG, status);
	decodeConfiguration(configuration, confdata);
}

void
WorkerSimulation::receiveNeurons()
{
	unsigned neuronCount, neuronLength;
	MPI::COMM_WORLD.Recv(&neuronCount, 1, MPI::INT, MASTER, NEURON_COUNT_TAG, status);
	for (unsigned nidx = 0; nidx < neuronCount; ++nidx) {
		MPI::COMM_WORLD.Recv(&neuronLength, 1, MPI::INT, MASTER, NEURON_LENGTH_TAG, status);
		char neuronData [neuronLength];
		MPI::COMM_WORLD.Recv(&neuronData, neuronLength, MPI::CHAR, MASTER, NEURON_DATA_TAG, status);
		parseNeuron(network,neuronData);
	}
}

void
WorkerSimulation::receiveSynapses()
{
	/* Synapse distribution */
}
