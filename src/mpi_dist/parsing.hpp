#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>
#include <nemo/Network.hpp>
#include <nemo/NetworkImpl.hpp>
#include <nemo/StdpFunction.hpp>
#include <nemo/Configuration.hpp>
#include <nemo/ConfigurationImpl.hpp>
#include <nemo/types.hpp>
#include <nemo/config.h>
#include <nemo/network/Generator.hpp>
using namespace std;

string encodeNeuron(float a, float b, float c, float d, float u, float v, float sigma, unsigned nidx);
string encodeNeuron(float* args, unsigned nidx);
float* decodeNeuron(const string& neuronData);

string encodeConfiguration(const nemo::Configuration &conf);
void decodeConfiguration(nemo::Configuration &target, const string& confData);

string encodeSTDP(nemo::StdpFunction stdp);
void decodeSTDP(nemo::Configuration &target, const string& stdp);

string encodeSynapse(const nemo::network::synapse_iterator& s, unsigned type);

vector<string> decode(const string& data, const string&	 delim);

#endif
