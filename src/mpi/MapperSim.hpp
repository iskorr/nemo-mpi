/* This is a mapper class that would define the clusterization of the network
 */

// The implementation of the cluster mapping algorithm is going to be here.
// Then depending on the worker id, the particular set of neurons could be called.

#include <iostream>
#include <sstream>
#include <Neuron.hpp>
#include <MasterSimulation.hpp>
//#include <parsing.hpp>

using namespace std;


class MapperSim
{
public:
	MapperSim(nemo::Network *net, unsigned workercount);
	vector<string> retrieveNeurons(unsigned worker);

private:
	vector<vector<unsigned>> neurons;
	nemo::Network* net;
	string encodeNeuron(Neuron neuron);
};
