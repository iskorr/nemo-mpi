FIND_PACKAGE(MPI)

IF(NOT MPI_FOUND)
	MESSAGE(SEND_ERROR "MPI enabled, but no MPI implementation found on this system")
ENDIF(NOT MPI_FOUND)

INCLUDE_DIRECTORIES(
	${Boost_INCLUDE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_SOURCE_DIR}/src
)

ADD_LIBRARY(nemo_mpi_dist SHARED
	MasterSimulation.cpp
	WorkerSimulation.cpp
	MapperSim.cpp
)

SET_TARGET_PROPERTIES(nemo_mpi_dist PROPERTIES
		DEFINE_SYMBOL NEMO_EXPORTS
)

TARGET_LINK_LIBRARIES(nemo_mpi_dist nemo ${LTDL_LIBRARY} ${Boost_LIBRARIES})

SET(BUILD_SHARED_LIBS TRUE)
INSTALL(FILES
	MasterSimulation.cpp
	WorkerSimulation.cpp
	MapperSim.cpp
	DESTINATION ${INSTALL_INCLUDE_DIR}/mpi_dist
)
