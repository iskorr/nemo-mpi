#include <vector>
#include <string>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <boost/scoped_ptr.hpp>
#include <boost/random.hpp>
#include <examples/common.hpp>
#include <nemo.hpp>
#include <nemo/parsing.hpp>
#include <nemo/MasterSimulation.hpp>
#include <nemo/WorkerSimulation.hpp>
using namespace std;

typedef boost::mt19937 rng_t;
typedef boost::variate_generator<rng_t&, boost::uniform_real<double> > urng_t;
typedef boost::variate_generator<rng_t&, boost::uniform_int<> > uirng_t;

void
addExcitatoryNeuron(nemo::Network* net, unsigned nidx, urng_t& param)
{
	float v = -65.0f;
	float a = 0.02f;
	float b = 0.2f;
	float r1 = float(param());
	float r2 = float(param());
	float c = v + 15.0f * r1 * r1;
	float d = 8.0f - 6.0f * r2 * r2;
	float u = b * v;
	float sigma = 5.0f;
	net->addNeuron(nidx, a, b, c, d, u, v, sigma);
}



void
addInhibitoryNeuron(nemo::Network* net, unsigned nidx, urng_t& param)
{
	float v = -65.0f;
	float r1 = float(param());
	float a = 0.02f + 0.08f * r1;
	float r2 = float(param());
	float b = 0.25f - 0.05f * r2;
	float c = v;
	float d = 2.0f;
	float u = b * v;
	float sigma = 2.0f;
	net->addNeuron(nidx, a, b, c, d, u, v, sigma);
}



nemo::Network*
construct(unsigned ncount, unsigned scount, unsigned dmax, bool stdp)
{
	rng_t rng;
	/* Neuron parameters and weights are partially randomised */
	urng_t randomParameter(rng, boost::uniform_real<double>(0, 1));
	uirng_t randomTarget(rng, boost::uniform_int<>(0, ncount-1));
	uirng_t randomDelay(rng, boost::uniform_int<>(1, dmax));

	nemo::Network* net = new nemo::Network();

	for(unsigned nidx=0; nidx < ncount; ++nidx) {
		if(nidx < (ncount * 4) / 5) { // excitatory
			addExcitatoryNeuron(net, nidx, randomParameter);
			for(unsigned s = 0; s < scount; ++s) {
				net->addSynapse(nidx, randomTarget(), randomDelay(), 0.5f * float(randomParameter()), stdp);
			}
		} else { // inhibitory
			addInhibitoryNeuron(net, nidx, randomParameter);
			for(unsigned s = 0; s < scount; ++s) {
				net->addSynapse(nidx, randomTarget(), 1U, float(-randomParameter()), 0);
			}
		}
	}
	return net;
}

int
main(int argc, char* argv[])
{
	unsigned neurons = 100, synapses = 50, duration = 1000, dmax = 1;
	MPI::Init(argc, argv);
	if (argc > 1) neurons = atoi(argv[1]);
	if (argc > 2) synapses = atoi(argv[2]);
	if (argc > 3) duration = atoi(argv[3]);
	if (argc > 4) dmax = atoi(argv[4]);
	unsigned rank = MPI::COMM_WORLD.Get_rank();
	unsigned workers = MPI::COMM_WORLD.Get_size();
	namespace po = boost::program_options;
	if (rank == 0) {
		nemo::Network* net(construct(neurons, synapses, dmax, false));
		nemo::Configuration conf;
		conf.setWriteOnlySynapses();
		conf.enableLogging();
		conf.setCpuBackend();
		MasterSimulation(*net,conf, duration);
	} else {
		WorkerSimulation(rank, workers);
	}
	MPI::Finalize();
	return 0;
}
