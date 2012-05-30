/* Simple network with 1000 neurons with all-to-all connections with random
 * weights.

 * Author: Andreas K. Fidjeland <andreas.fidjeland@imperial.ac.uk>
 * Date: April 2010
 */

#include <vector>
#include <string>
#include <iostream>
#include <fstream>

#include <mpi.h>
#include <boost/program_options.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/random.hpp>

#include <examples/common.hpp>
#include <nemo.hpp>

using namespace std;

typedef boost::mt19937 rng_t;
typedef boost::variate_generator<rng_t&, boost::uniform_real<double> > urng_t;
typedef boost::variate_generator<rng_t&, boost::uniform_int<> > uirng_t;

struct null_streambuf: public std::streambuf
{
	void overflow(char c)
	{
	}
};

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
	MPI::Init ( argc, argv );
	MPI::Status status;
	unsigned workers = MPI::COMM_WORLD.Get_size();
	unsigned rank = MPI::COMM_WORLD.Get_rank();
	unsigned neuronCount,synapsesPerNeuron;

	null_streambuf nullbuf;
	std::ostream out(&nullbuf);

	if (rank == 0) {

		neuronCount = 100;
		unsigned simRun;
		unsigned worker = 1;
		unsigned neuronOffset = neuronCount % (workers-1);
		unsigned neuronCountPerNetwork = neuronCount / (workers-1);
		unsigned lastNeuronCount = neuronCountPerNetwork + neuronOffset;
		unsigned synapsesPerNeuron = neuronCountPerNetwork / 2;

		cout << "Blocking Random Simulation initiated" << endl;

		for (; worker < workers-1; ++worker) {
			MPI::COMM_WORLD.Send(&neuronCountPerNetwork, 1, MPI::INT, worker, (int) 0);
			MPI::COMM_WORLD.Send(&synapsesPerNeuron, 1, MPI::INT, worker, (int) 1);
		}
		if (workers > 1) {
			MPI::COMM_WORLD.Send(&lastNeuronCount, 1, MPI::INT, workers-1, (int) 0);
			MPI::COMM_WORLD.Send(&synapsesPerNeuron, 1, MPI::INT, workers-1, (int) 1);		
		}
		cout << "All workers are set up, running simulations" << endl;


		worker = 1;
		for (; worker < workers; ++worker) {
			MPI::COMM_WORLD.Recv(&simRun, 1, MPI::INT, worker, (int) 2, status);
			if (simRun == 0) break;
		}

		if (worker >= workers-1) cout << "Simulations were run successfully!" << endl;
		else cout << "There is a problem with a worker " << worker << endl;

	} else if (rank > 0) {

		MPI::COMM_WORLD.Recv(&neuronCount, 1, MPI::INT, 0, (int) 0, status);
		MPI::COMM_WORLD.Recv(&synapsesPerNeuron, 1, MPI::INT, 0, (int) 1, status);
		cout << "Neurons on the machine " << rank << " are " << neuronCount << endl;
		cout << "Synapses on the machine " << rank << " are " << synapsesPerNeuron << endl;
		namespace po = boost::program_options;
		int reply;

		try {
			po::options_description desc = commonOptions();
			desc.add_options()
				("neurons,n", po::value<unsigned>()->default_value(neuronCount), "number of neurons")
				("synapses,m", po::value<unsigned>()->default_value(synapsesPerNeuron), "number of synapses per neuron")
				("dmax,d", po::value<unsigned>()->default_value(1), "maximum excitatory delay,  where delays are uniform in range [1, dmax]")
			;
	
			po::variables_map vm = processOptions(argc, argv, desc);

			unsigned dmax = vm["dmax"].as<unsigned>();
			unsigned duration = vm["duration"].as<unsigned>();
			
			cout << "Constructing network " << rank << endl;
			boost::scoped_ptr<nemo::Network> net(nemo::random::construct(neuronCount, synapsesPerNeuron, dmax));
			cout << "Creating configuration " << rank << endl;
			nemo::Configuration conf = configuration(vm);
			cout << "Creating simulation " << rank << endl;
			boost::scoped_ptr<nemo::Simulation> sim(nemo::simulation(*net, conf));
			cout << "Running simulation" << rank << endl;
			simulate(sim.get(), duration, 0, out);
			reply = 1;
			MPI::COMM_WORLD.Send(&reply, 1, MPI::INT, 0, (int) 2);
		} catch(...) {
			cerr << "random: An unknown error occurred\n";
			reply = 0;
			MPI::COMM_WORLD.Send(&reply, 1, MPI::INT, 0, (int) 2);
		}
	}
	MPI::Finalize();
	return 0;
}

#endif
