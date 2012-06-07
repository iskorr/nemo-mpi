/* Simple MPI blocking random network simulation */
#define MASTER 0
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

using namespace std;

typedef boost::mt19937 rng_t;
typedef boost::variate_generator<rng_t&, boost::uniform_real<double> > urng_t;
typedef boost::variate_generator<rng_t&, boost::uniform_int<> > uirng_t;

string
createNeuron(unsigned nidx, urng_t& param)
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
	string neuronData = ::encodeNeuron(a,b,c,d,u,v,sigma,nidx);	
	return neuronData;
}

void
parseNeuron(nemo::Network* net, const string& neuronData)
{
	float* res = decodeNeuron(neuronData);
	net->addNeuron((unsigned)res[7], res[0], res[1], res[2], res[3], res[4], res[5], res[6]);
}

void
distributeNeuronAndSynapseData(unsigned ncount, unsigned scount, unsigned worker, urng_t param)
{
	string neuronData;
	unsigned strlen;
	MPI::COMM_WORLD.Send(&ncount, 1, MPI::INT, worker, (int) 0);
	MPI::COMM_WORLD.Send(&scount, 1, MPI::INT, worker, (int) 1);
	for (unsigned i = 0; i < ncount; i++) {
		string neuData = createNeuron(i,param);
		strlen = neuData.size()+1;		
		char msg [strlen];
		msg[strlen-1] = 0;
		memcpy(msg, neuData.c_str(), strlen-1);
		MPI::COMM_WORLD.Send(&strlen,1,MPI::INT,worker,(int) 2);
		MPI::COMM_WORLD.Send(&msg,strlen,MPI::CHAR, worker, (int) 3);
	}
}

void
masterRoutine(unsigned neuronCount, rng_t rng, MPI::Status status)
{
	unsigned workers = MPI::COMM_WORLD.Get_size();
	unsigned simRun, worker = 1;
	if (workers == 1) return;
	unsigned neuronCountPerNetwork = neuronCount / (workers-1);
	unsigned lastNeuronCount = neuronCountPerNetwork + (neuronCount % (workers-1));
	unsigned synapsesPerNeuron = neuronCountPerNetwork;
	urng_t randomParameter(rng, boost::uniform_real<double>(0, 1));

	cout << "Blocking Random Simulation initiated" << endl;
	for (; worker < workers-1; ++worker) {
		distributeNeuronAndSynapseData(neuronCountPerNetwork, synapsesPerNeuron, worker, randomParameter);
	}
	if (workers > 1) {
		distributeNeuronAndSynapseData(lastNeuronCount, synapsesPerNeuron, worker, randomParameter);
	}
	cout << "All workers are set up, running simulations" << endl;

	worker = 1;
	for (; worker < workers; ++worker) {
		MPI::COMM_WORLD.Recv(&simRun, 1, MPI::INT, worker, (int) 4, status);
		if (simRun == 0) break;
	}
	if (worker >= workers-1) cout << "Simulations were run successfully!" << endl;
	else cout << "There is a problem with a worker " << worker << endl;
}

void
workerRoutine(unsigned duration, unsigned rank, rng_t rng, MPI::Status status)
{
	unsigned neuronCount,synapsesPerNeuron,strlen,reply;
	MPI::COMM_WORLD.Recv(&neuronCount, 1, MPI::INT, MASTER, (int) 0, status);
	MPI::COMM_WORLD.Recv(&synapsesPerNeuron, 1, MPI::INT, MASTER, (int) 1, status);
	try {
		/* Random neuron parameters */
		urng_t randomParameter(rng, boost::uniform_real<double>(0, 1));
		uirng_t randomTarget(rng, boost::uniform_int<>(0, neuronCount-1));
		uirng_t randomDelay(rng, boost::uniform_int<>(1, 1));
		nemo::Network* network = new nemo::Network();

		for(unsigned nidx=0; nidx < neuronCount; ++nidx) {
			MPI::COMM_WORLD.Recv(&strlen, 1, MPI::INT, MASTER, (int) 2, status);
			char msg [strlen];
			MPI::COMM_WORLD.Recv(&msg, strlen, MPI::CHAR, MASTER, (int) 3, status);
			parseNeuron(network,msg);
		}
		for(unsigned nidx=0; nidx < neuronCount; ++nidx) network->addSynapse(nidx, randomTarget(), randomDelay(), 0.5f * float(randomParameter()), false);
		nemo::Configuration conf;
		conf.setWriteOnlySynapses();
		conf.enableLogging();
		conf.setCpuBackend();
		
		cout << "Constructing network " << rank << endl;
		boost::scoped_ptr<nemo::Network> net(network);
		cout << "Creating simulation " << rank << endl;
		boost::scoped_ptr<nemo::Simulation> sim(nemo::simulation(*network, conf));
		cout << "Running simulation " << rank << endl;
		simulate(sim.get(), duration, 0, cout);
		reply = 1;
		MPI::COMM_WORLD.Send(&reply, 1, MPI::INT, MASTER, (int) 4);
	} catch(...) {
		cerr << "random: An unknown error occurred\n";
		reply = 0;
		MPI::COMM_WORLD.Send(&reply, 1, MPI::INT, MASTER, (int) 4);
	}
}

int
main(int argc, char* argv[])
{
	unsigned ncount, duration;
	if (argc <= 1) ncount = 100;
	else ncount = atoi(argv[1]);
	if (argc <= 2) duration = 500;
	else duration = atoi(argv[2]);
	cout << ncount << " " << duration << endl;
	MPI::Init ( argc, argv );
	MPI::Status status;
	rng_t rng;
	unsigned rank = MPI::COMM_WORLD.Get_rank();
	if (rank == MASTER) {
		masterRoutine(ncount, rng, status);
	} else if (rank > MASTER) {
		workerRoutine(duration, rank, rng, status);
	}
	MPI::Finalize();
	return 0;
}
