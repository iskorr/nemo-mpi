#include "parsing.hpp"
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
using namespace std;

string
encodeNeuron(float a, float b, float c, float d, float u, float v, float sigma, unsigned nidx)
{
	ostringstream input;
	input << a << "," << b << "," << c << "," << d << "," << u << "," << v << "," << sigma << "," << nidx;
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
