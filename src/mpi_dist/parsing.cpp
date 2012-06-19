#include "parsing.hpp"
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
using namespace std;

string
encodeConfiguration(const nemo::Configuration &conf)
{
	unsigned logging = 0;
	if (conf.loggingEnabled()) logging = 1;
	unsigned fract_bits = conf.m_impl->fractionalBits();
	unsigned cuda_part_size = conf.cudaPartitionSize();
	unsigned backend = 0;
	if (conf.backend() == NEMO_BACKEND_CUDA) backend = 1;
	const char* description = conf.backendDescription();
	ostringstream input;
	input << logging << "<>" << fract_bits << "<>" << cuda_part_size << "<>" << backend << "<>" << description;
	boost::optional<nemo::StdpFunction> stdp;
	try {
	//	stdp = *conf.m_impl->stdpFunction();
	} catch (...) {
	}
	if (stdp) input << encodeSTDP(*conf.m_impl->stdpFunction());
	else input << "<>NULL";
	string neuronData(input.str());
	return neuronData;
}

void
decodeConfiguration(nemo::Configuration &target, const string& confData)
{
	vector<string> properties = decode(confData,"<>");
	float result [properties.size()];
	for (unsigned i = 0; i < properties.size()-1; ++i) result[i] = ::atof(properties[i].c_str());
	if (result[0] == 1) target.enableLogging();
	else target.disableLogging();
	target.m_impl->setFractionalBits(result[1]);
	target.m_impl->setCudaPartitionSize(result[2]);
	if (properties[properties.size()-1].compare("NULL") != 0) {
		decodeSTDP(target,properties[properties.size()-1]);
	}
}

string
encodeSTDP(nemo::StdpFunction stdp)
{
	ostringstream input;
	vector<float> prefire = stdp.prefire();
	vector<float> postfire = stdp.postfire();
	for (unsigned i = 0; i < 20; ++i) input << prefire[i] << ",";
	for (unsigned i = 0; i < 20; ++i) input << postfire[i] << ",";
	input << stdp.maxInhibitoryWeight() << "," << stdp.minInhibitoryWeight() << "," << stdp.maxExcitatoryWeight() << "," << stdp.minExcitatoryWeight();
	string stdpData(input.str());
	return stdpData;
}

void
decodeSTDP(nemo::Configuration &target, const string& stdp)
{
	vector<string> properties = decode(stdp,",");
	float result [44];
	for (unsigned i = 0; i < properties.size(); ++i) result[i] = ::atof(properties[i].c_str());
	vector<float> prefire;
	vector<float> postfire;
	for (unsigned i = 0; i < 20; ++i) prefire[i] = result[i];
	for (unsigned i = 20; i < 40; ++i) postfire[i] = result[i];
	float maxIW = result[40];
	float minIW = result[41];
	float maxEW = result[42];
	float minEW = result[43];
	target.setStdpFunction(prefire,postfire,minEW,maxEW,minIW,maxIW);
}

float*
encodeSynapse(int source, int target, unsigned delay, float weight, unsigned plastic)
{
	float* res = new float [5];
	res[0] = source;
	res[1] = target;
	res[2] = delay;
	res[3] = weight;
	res[4] = plastic;
	return res;
}

vector<string>
encodeMapper(nemo::mpi_dist::MapperSim& mapper)
{
	unsigned workers = mapper.workerCount();
	vector<string> res;
	res.resize(workers);
	for (unsigned i = 0; i < workers; ++i) {
		ostringstream input;
		vector<unsigned> neurons = mapper.retrieveNeurons(i+1);
		if (!neurons.empty())input << neurons[0];
		for (unsigned j = 1; j < neurons.size(); ++j) input << "," << neurons[j];
		string mapData(input.str());
		res[i] = mapData;
	}
	return res;
}

vector<string>
decode(const string& data, const string& delim)
{
	vector<string> properties;
  	size_t p0 = 0, p1 = string::npos;
	while(p0 != string::npos) {
		p1 = data.find_first_of(delim, p0);
		if(p1 > p0)
    		{
      			string prop = data.substr(p0, p1 - p0);
      			properties.push_back(prop);
    		}
    		p0 = data.find_first_not_of(delim, p1);
 	}
	return properties;
}
