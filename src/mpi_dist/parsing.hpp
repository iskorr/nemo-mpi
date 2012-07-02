/* Copyright 2012 Imperial College London */
/* Parser class accessible by both Master and Worker simulation classes - now mostly redundant */
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

std::string encodeConfiguration(const nemo::Configuration &conf);
void decodeConfiguration(nemo::Configuration &target, const std::string& confData);

std::string encodeSTDP(nemo::StdpFunction stdp);
void decodeSTDP(nemo::Configuration &target, const std::string& stdp);

float* encodeSynapse(int source, int target, unsigned delay, float weight, unsigned plastic);
std::vector<std::string> encodeMapper(nemo::mpi_dist::MapperSim& mapper);

std::vector<std::string> decode(const std::string& data, const std::string& delim);

#endif
