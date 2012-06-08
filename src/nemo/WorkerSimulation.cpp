#include "mpi.h"
#include "WorkerSimulation.hpp"
#include "mpi_tags.hpp"
#include <vector>
#include <string>
#include "parsing.hpp"

WorkerSimulation::WorkerSimulation()
{
	unsigned reply;
	rank = MPI::COMM_WORLD.Get_rank();
	nemo::Network* net = new nemo::Network();
	nemo::Configuration conf;
	cout << "Start Worker" << endl;
	receiveConfiguration(conf);
	cout << "Conf done Worker" << endl;
	receiveNeurons(net);
	cout << "Neurons done Worker" << endl;
	receiveSynapses(net);
	cout << "Synapses done Worker" << endl;
	reply = 1;
	MPI::COMM_WORLD.Send(&reply, 1, MPI::INT, MASTER, DISTRIBUTION_COMPLETE);
}

WorkerSimulation::~WorkerSimulation()
{
	;
}

void
WorkerSimulation::receiveConfiguration(nemo::Configuration& configuration)
{
	unsigned confLength;
	MPI::COMM_WORLD.Recv(&confLength, 1, MPI::INT, MASTER, CONFIGURATION_LENGTH_TAG, status);	
	char confdata [confLength];
	MPI::COMM_WORLD.Recv(&confdata, confLength, MPI::CHAR, MASTER, CONFIGURATION_DATA_TAG, status);
	decodeConfiguration(configuration, confdata);
}

void
WorkerSimulation::receiveNeurons(nemo::Network* net)
{
	unsigned neuronCount, neuronLength;
	MPI::COMM_WORLD.Recv(&neuronCount, 1, MPI::INT, MASTER, NEURON_COUNT_TAG, status);
	for (unsigned nidx = 0; nidx < neuronCount; ++nidx) {
		MPI::COMM_WORLD.Recv(&neuronLength, 1, MPI::INT, MASTER, NEURON_LENGTH_TAG, status);
		char neuronData [neuronLength];
		MPI::COMM_WORLD.Recv(&neuronData, neuronLength, MPI::CHAR, MASTER, NEURON_DATA_TAG, status);
		float* neuronParameters = decodeNeuron(neuronData);
		net->addNeuron((unsigned)neuronParameters[7], neuronParameters[0], neuronParameters[1], neuronParameters[2], 
				neuronParameters[3], neuronParameters[4], neuronParameters[5], neuronParameters[6]);
	}
}

void
WorkerSimulation::receiveSynapses(nemo::Network* net)
{
	unsigned ok,synLength;
	while(true) {
		MPI::COMM_WORLD.Recv(&ok,1,MPI::INT,MASTER,SYNAPSE_END_TAG,status);
		if (ok == 1) break;
		MPI::COMM_WORLD.Recv(&synLength, 1, MPI::INT, MASTER, SYNAPSE_LENGTH_TAG, status);
		char synData [synLength];
		MPI::COMM_WORLD.Recv(&synData, synLength, MPI::CHAR, MASTER, SYNAPSE_DATA_TAG, status);
		decodeSynapse(net, synData);
	}
}
