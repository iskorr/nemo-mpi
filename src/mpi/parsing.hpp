#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>
#include <nemo/StdpFunction.hpp>
#include <nemo/Configuration.hpp>
#include <nemo/ConfigurationImpl.hpp>
using namespace std;

string encodeNeuron(float a, float b, float c, float d, float u, float v, float sigma, unsigned nidx);
float* decodeNeuron(string neuronData);

string encodeConfiguration(nemo::Configuration &conf);
void decodeConfiguration(nemo::Configuration &target, string confData);

string encodeSTDP(nemo::StdpFunction stdp);
void decodeSTDP(nemo::Configuration &target, string stdp);

vector<string> decode(string data, string delim);
char* buffer(string data, unsigned& strlen);

#endif
