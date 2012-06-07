#include "parsing.hpp"
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <nemo/StdpFunction.hpp>
#include <nemo/Configuration.hpp>
#include <nemo/ConfigurationImpl.hpp>
using namespace std;

string
encodeNeuron(float a, float b, float c, float d, float u, float v, float sigma, unsigned nidx)
{
	ostringstream input;
	input << a << "," << b << "," << c << "," << d << "," << u << "," << v << "," << sigma << "," << nidx;
	string neuronData(input.str());
	return neuronData;
}

string
encodeNeuron(float* args)
{
	ostringstream input;
	input << args[0];
	for (unsigned i = 1; i < 7; ++i) input << "," << args[i];
	string neuronData(input.str());
	return neuronData;
}

float*
decodeNeuron(const string& neuronData)
{
	vector<string> properties = decode(neuronData,",");
	float* result = new float [properties.size()];
	for (unsigned i = 0; i < properties.size(); ++i) result[i] = ::atof(properties[i].c_str());
	return result;
}

string
encodeConfiguration(nemo::Configuration &conf)
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
	//input << encodeSTDP(*conf.m_impl->stdpFunction());
	string neuronData(input.str());
	return neuronData;
}

void
decodeConfiguration(nemo::Configuration &target, const string& confData)
{
	vector<string> properties = decode(confData,"<>");
	float result [properties.size()];
	for (unsigned i = 0; i < properties.size()-1; ++i) cout << properties[i] << endl;
	for (unsigned i = 0; i < properties.size()-1; ++i) result[i] = ::atof(properties[i].c_str());
	if (result[0] == 1) target.enableLogging();
	else target.disableLogging();
	target.m_impl->setFractionalBits(result[1]);
	target.m_impl->setCudaPartitionSize(result[2]);
/*	if (properties[properties.size()-1].compare("NULL") != 0) {
		decodeSTDP(target,properties[properties.size()-1]);
	}*/
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
	float result [properties.size()];
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
