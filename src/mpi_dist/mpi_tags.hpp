/* Copyright 2012 Imperial College London */
/* MPI tags that are used for communication - accessible by both Master and Worker simulation classes */
#ifndef MPI_TAGS_HPP
#define MPI_TAGS_HPP
#define MASTER 0
#define NEURON_ARGS 8
#define SYNAPSE_ARGS 5
#define CONFIGURATION_LENGTH_TAG 0
#define CONFIGURATION_DATA_TAG 1
#define NEURON_COUNT_TAG 2
#define NEURON_LENGTH_TAG 3
#define NEURON_DATA_TAG 4
#define SYNAPSE_LENGTH_TAG 5
#define SYNAPSE_DATA_TAG 6
#define SYNAPSE_END_TAG 7
#define DISTRIBUTION_COMPLETE 8
#define SIM_STEP 9
#define COMMUNICATION_TAG 10
#define TOTAL_NEURONS_TAG 11
#define MAPPER_LENGTH_TAG 12
#define MAPPER_DATA_TAG 13
#define SPIKES 14
#endif
