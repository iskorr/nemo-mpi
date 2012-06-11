#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>
#include <vector>
using namespace std;

string encodeNeuron(float a, float b, float c, float d, float u, float v, float sigma, unsigned nidx);
float* decodeNeuron(const string& neuronData);
vector<string> decode(const string& data, const string&	 delim);

#endif
