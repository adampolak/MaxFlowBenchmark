cmake_minimum_required(VERSION 3.16)
project(MaxFlowBenchmark)

set(CMAKE_CXX_STANDARD 14)


include_directories(deps/lemon)
include_directories(cmake-build-debug/deps/lemon)
include_directories(.)
include_directories(build/deps/lemon)

FIND_PATH(LEMON_SOURCE_ROOT_DIR CMakeLists.txt
        PATHS ${CMAKE_SOURCE_DIR}/lemon ${CMAKE_SOURCE_DIR}/deps/lemon
        NO_DEFAULT_PATH
        DOC "Location of LEMON source as a CMAKE subproject")

ADD_SUBDIRECTORY(${LEMON_SOURCE_ROOT_DIR} deps/lemon)

SET(LEMON_INCLUDE_DIRS
        ${LEMON_SOURCE_ROOT_DIR}
        ${CMAKE_BINARY_DIR}/deps/lemon)
SET(LEMON_LIBRARIES lemon)

ADD_EXECUTABLE(MaxFlowBenchmark main.cpp learning/learning.cpp learning/learning.h distribution.h algorithms/preflow.h algos.h algorithms/learning_augmented_add_edges_lemon.h algorithms/learning_augmented_paths_removal_lemon.h algorithms/learning_augmented_lower_bounds_lemon.h)
TARGET_LINK_LIBRARIES(MaxFlowBenchmark lemon)
