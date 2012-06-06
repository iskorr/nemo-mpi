#include "parsing.hpp"
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
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
decodeNeuron(string neuronData)
{
	vector<string> properties;
  	size_t p0 = 0, p1 = string::npos;
	while(p0 != string::npos) {
		p1 = neuronData.find_first_of(",", p0);
		if(p1 > p0)
    		{
      			string prop = neuronData.substr(p0, p1 - p0);
      			properties.push_back(prop);
    		}
    		p0 = neuronData.find_first_not_of(",", p1);
 	}
	float result [properties.size()];
	for (unsigned i = 0; i < properties.size(); ++i) result[i] = ::atof(properties[i].c_str());
	return result;
}
