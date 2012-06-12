#ifndef PARSER_HPP
#define PARSER_HPP
#include <string>
#include <mpi_dist/MapperSim.hpp>
#include <nemo/Network.hpp>
#include <nemo/NetworkImpl.hpp>
#include <nemo/StdpFunction.hpp>
#include <nemo/Configuration.hpp>
#include <nemo/ConfigurationImpl.hpp>
#include <nemo/types.hpp>
#include <nemo/config.h>
#include <nemo/network/Generator.hpp>

std::string encodeNeuron(float* args, unsigned nidx);
float* decodeNeuron(const std::string& neuronData);

std::string encodeConfiguration(const nemo::Configuration &conf);
void decodeConfiguration(nemo::Configuration &target, const std::string& confData);

std::string encodeSTDP(nemo::StdpFunction stdp);
void decodeSTDP(nemo::Configuration &target, const std::string& stdp);

std::string encodeSynapse(const nemo::network::synapse_iterator& s, unsigned type);
std::vector<std::string> encodeMapper(nemo::mpi_dist::MapperSim& mapper);

std::vector<std::string> decode(const std::string& data, const std::string& delim);

#endif
