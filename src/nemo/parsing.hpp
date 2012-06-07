#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>
#include <nemo/StdpFunction.hpp>
#include <nemo/Configuration.hpp>
#include <nemo/ConfigurationImpl.hpp>
using namespace std;

string encodeNeuron(float a, float b, float c, float d, float u, float v, float sigma, unsigned nidx);
string encodeNeuron(float* args);
float* decodeNeuron(const string& neuronData);

string encodeConfiguration(nemo::Configuration &conf);
void decodeConfiguration(nemo::Configuration &target, const string& confData);

string encodeSTDP(nemo::StdpFunction stdp);
void decodeSTDP(nemo::Configuration &target, const string& stdp);

vector<string> decode(const string& data, const string&	 delim);

#endif
