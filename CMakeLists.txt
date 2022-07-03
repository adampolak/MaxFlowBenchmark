cmake_minimum_required(VERSION 3.16)
project(MaxFlowBenchmark)

set(CMAKE_CXX_STANDARD 14)

add_executable(MaxFlowBenchmark main.cpp algorithms/algorithm.cpp algorithms/algorithm.h learning/learning.cpp learning/learning.h random_util.cpp random_util.h algorithms/preflow.cpp algorithms/preflow.h algos.cpp algos.h algorithms/edmonds_carp.cpp algorithms/edmonds_carp.h algorithms/learning_augmented_add_edges_lemon.cpp algorithms/learning_augmented_add_edges_lemon.h algorithms/learning_augmented_paths_removal_lemon.cpp algorithms/learning_augmented_paths_removal_lemon.h dimacs_input.cpp dimacs_input.h)

include_directories(deps/lemon)
include_directories(cmake-build-debug/deps/lemon)
include_directories(.)
include_directories(build/deps/lemon)

FIND_PATH(LEMON_SOURCE_ROOT_DIR CMakeLists.txt
        PATHS ${CMAKE_SOURCE_DIR}/lemon ${CMAKE_SOURCE_DIR}/deps/lemon
        NO_DEFAULT_PATH
        DOC "Location of LEMON source as a CMAKE subproject")



message(STATUS ${LEMON_SOURCE_ROOT_DIR}kek)

ADD_SUBDIRECTORY(${LEMON_SOURCE_ROOT_DIR} deps/lemon)

SET(LEMON_INCLUDE_DIRS
        ${LEMON_SOURCE_ROOT_DIR}
        ${CMAKE_BINARY_DIR}/deps/lemon)
SET(LEMON_LIBRARIES lemon)


