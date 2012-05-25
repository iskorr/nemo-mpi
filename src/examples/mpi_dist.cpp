/* Simple network with 1000 neurons with all-to-all connections with random
 * weights.

 * Author: Andreas K. Fidjeland <andreas.fidjeland@imperial.ac.uk>
 * Date: April 2010
 */

#include <vector>
#include <string>
#include <iostream>
#include <fstream>

#include <boost/mpi/environment.hpp>
#include <boost/mpi/communicator.hpp>
#include <boost/program_options.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/random.hpp>

#include <examples/common.hpp>
#include <nemo.hpp>
namespace mpi = boost::mpi;
using namespace std;

typedef boost::mt19937 rng_t;
typedef boost::variate_generator<rng_t&, boost::uniform_real<double> > urng_t;
typedef boost::variate_generator<rng_t&, boost::uniform_int<> > uirng_t;

namespace nemo {
	namespace random {

void
addNeuron(nemo::Network* net, unsigned nidx, urng_t& param)
{
	float v = -65.0f;
	float a = 0.02f;
	float b = 0.2f;
	float r1 = float(param());
	float r2 = float(param());
	float c = v + 15.0f*r1*r2;
	float d = 8.0f - 6.0f*r1*r2;
	float u = b * v;
	float sigma = 5.0f;
	net->addNeuron(nidx, a, b, c, d, u, v, sigma);
}

nemo::Network*
construct(unsigned ncount, unsigned scount, unsigned dmax)
{
	rng_t rng;
	/* Neuron parameters and weights are partially randomised */
	urng_t randomParameter(rng, boost::uniform_real<double>(0, 1));
	uirng_t randomTarget(rng, boost::uniform_int<>(0, ncount-1));
	uirng_t randomDelay(rng, boost::uniform_int<>(1, dmax));

	nemo::Network* net = new nemo::Network();

	for(unsigned nidx=0; nidx < ncount; ++nidx) {
		addNeuron(net, nidx, randomParameter);
		for(unsigned s = 0; s < scount; ++s) {
			net->addSynapse(nidx, randomTarget(), randomDelay(), 0.5f * float(randomParameter()), false);
		}
	}
	return net;
}

	} // namespace random
} // namespace nemo


#ifdef USING_MAIN


#define LOG(cond, ...) if(cond) { fprintf(stdout, __VA_ARGS__); fprintf(stdout, "\n"); }


int
main(int argc, char* argv[])
{

	mpi::environment = env(argc, argv);
	mpi::communicator world;
	unsigned rank = world.rank();
	unsigned neuronCount,synapsesPerNeuron;

	if (rank == 0) {

		unsigned workers  = world.size();
		unsigned worker = 1;
		neuronCount=1000;
		synapsesPerNeuron=200;

		for (; worker < workers; ++worker) {
			world.send(worker, 0, neuronCount);
			world.send(worker, 1, synapsesPerNeuron);
		}
		cout << "All workers are set up, running simulations" << endl;
	
		unsigned simRun;
		worker = 1;

		for (; worker < workers; ++worker) {
			world.recv(worker, 2, simRun);
			if (simRun == 0) break;
		}

		if (worker == workers-1) cout << "Simulations were run successfully!" << endl;
		else cout << "There is a problem with a worker " << worker << endl;

	} else if (rank > 0) {

		world.recv(0, 0, neuronCount);
		world.recv(0, 1, synapsesPerNeuron);
		namespace po = boost::program_options;

		try {
			po::options_description desc = commonOptions();
			desc.add_options()
				("neurons,n", po::value<unsigned>()->default_value(neuronCount), "number of neurons")
				("synapses,m", po::value<unsigned>()->default_value(synapsesPerNeuron), "number of synapses per neuron")
			;
	
			po::variables_map vm = processOptions(argc, argv, desc);
	
			unsigned ncount = vm["neurons"].as<unsigned>();
			unsigned scount = vm["synapses"].as<unsigned>();
			unsigned duration = vm["duration"].as<unsigned>();
	
			boost::scoped_ptr<nemo::Network> net(nemo::random::construct(ncount, scount,5));
			nemo::Configuration conf = configuration(vm);
			boost::scoped_ptr<nemo::Simulation> sim(nemo::simulation(*net, conf));
			simulate(sim.get(), duration, 0, cout);
			world.send(0, 2, 1);
			return 0;
		} catch(...) {
			cerr << "random: An unknown error occurred\n";
			world.send(0, 2, 0);
			return -1;
		}
	}
}

#endif
