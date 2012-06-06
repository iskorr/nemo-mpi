#include <vector>
#include <boost/program_options.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/random.hpp>

namespace nemo {

	namespace mpi {

BlockingWorker (MPI_Comm world)
{

	int numberOfNeurons;
	MPI_Recv(&numberOfNeurons, 1, MPI_INT, 0,1, MPI_COMM_WORLD, &stat);
	nemo::Network* net = nemo::construct(numberOfNeurons,numberOfNeurons/4);
	if (nemo == NULL) 	MPI_Send(0, 1, MPI_INT, 0,1, world);
	else 	MPI_Send(1, 1, MPI_INT, 0,1, world);

}

void
addDummyNeuron(nemo::Network* net, unsigned id)
{
	float v = -65.0f;
	float a = 0.02f;
	float b = 0.2f;
	float c = v + 15.0f;
	float d = 8.0f - 6.0f;
	float u = b * v;
	float sigma = 5.0f;
	net->addNeuron(id, a, b, c, d, u, v, sigma);
}

nemo::Network*
construct(unsigned ncount, unsigned scount)
{
	rng_t rng;
	/* Neuron parameters and weights are partially randomised */
	uirng_t randomTarget(rng, boost::uniform_int<>(0, ncount-1));
	uirng_t randomDelay(rng, boost::uniform_int<>(1, 2));

	nemo::Network* net = new nemo::Network();

	for(unsigned id=0; id < numberOfNeurons; id++) {
		addDummyNeuron(net, id);
		for(unsigned s = 0; s < numberOfSynapses; ++s) {
			net->addSynapse(nidx, randomTarget(), randomDelay(), 0.5f, false);
		}
	}
	return net;
}

	}

}
