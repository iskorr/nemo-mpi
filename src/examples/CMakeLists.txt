FIND_PACKAGE(Boost REQUIRED COMPONENTS program_options)
INCLUDE_DIRECTORIES(
	${CMAKE_SOURCE_DIR}/src
	${CMAKE_BINARY_DIR}/src
	${Boost_INCLUDE_DIR}
)

ADD_DEFINITIONS(-DUSING_MAIN)
LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})

LINK_STATIC_MSVCR()

ADD_EXECUTABLE(torus torus.cpp common.cpp)
TARGET_LINK_LIBRARIES(torus nemo nemo_mpi_dist ${Boost_LIBRARIES})

ADD_EXECUTABLE(random random.cpp common.cpp)
TARGET_LINK_LIBRARIES(random nemo ${Boost_LIBRARIES})

ADD_EXECUTABLE(kuramoto kuramoto.cpp common.cpp)
TARGET_LINK_LIBRARIES(kuramoto nemo ${Boost_LIBRARIES})

ADD_EXECUTABLE(mpi_dist mpi_dist.cpp parsing.cpp common.cpp)
TARGET_LINK_LIBRARIES(mpi_dist nemo ${Boost_LIBRARIES})

ADD_EXECUTABLE(mpi_dist_general mpi_dist_general.cpp common.cpp)
TARGET_LINK_LIBRARIES(mpi_dist_general nemo nemo_mpi_dist ${Boost_LIBRARIES})

SET(INSTALL_FILES torus.cpp random.cpp mpi_dist.cpp common.cpp)

IF(UNIX)
	SET(INSTALL_EXAMPLE_SRC_DIR "share/nemo/examples")
	SET(INSTALL_EXAMPLE_BIN_DIR "share/nemo/examples")
ENDIF(UNIX)

IF(WIN32)
	SET(INSTALL_EXAMPLE_SRC_DIR "examples")
	SET(INSTALL_EXAMPLE_BIN_DIR "examples")
ENDIF(WIN32)

INSTALL(FILES ${INSTALL_FILES} DESTINATION ${INSTALL_EXAMPLE_SRC_DIR})
INSTALL(TARGETS mpi_dist_general mpi_dist random torus DESTINATION ${INSTALL_EXAMPLE_BIN_DIR})
