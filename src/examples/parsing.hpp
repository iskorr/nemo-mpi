#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>
using namespace std;

string encodeNeuron(float a, float b, float c, float d, float u, float v, float sigma, unsigned nidx);

float* decodeNeuron(string neuronData);

#endif
