cmake_minimum_required(VERSION 3.21)
project(MaxFlowBenchmark)

set(CMAKE_CXX_STANDARD 14)

find_package(Boost COMPONENTS graph REQUIRED)

add_executable(MaxFlowBenchmark main.cpp algorithms/algorithm.cpp algorithms/algorithm.h algorithms/boykov_kolmogorov.cpp algorithms/boykov_kolmogorov.h algorithms/push_relabel.cpp algorithms/push_relabel.h algorithms/learning_augmented_add_edges.cpp algorithms/learning_augmented_add_edges.h learning/learning.cpp learning/learning.h algorithms/learning_augmented_paths_removal.cpp algorithms/learning_augmented_paths_removal.h learning/random_util.cpp learning/random_util.h)

target_link_libraries(MaxFlowBenchmark Boost::graph)

include_directories(deps/lemon)
include_directories(cmake-build-debug/deps/lemon)
include_directories(.)

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


