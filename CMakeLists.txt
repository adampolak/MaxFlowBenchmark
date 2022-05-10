cmake_minimum_required(VERSION 3.21)
project(MaxFlowBenchmark)

set(CMAKE_CXX_STANDARD 14)

find_package(Boost COMPONENTS graph REQUIRED)

add_executable(MaxFlowBenchmark main.cpp algorithms/algorithm.cpp algorithms/algorithm.h algorithms/boykov_kolmogorov.cpp algorithms/boykov_kolmogorov.h algorithms/push_relabel.cpp algorithms/push_relabel.h algorithms/learning_augmented_add_edges.cpp algorithms/learning_augmented_add_edges.h learning/learning.cpp learning/learning.h algorithms/learning_augmented_paths_removal.cpp algorithms/learning_augmented_paths_removal.h learning/random_util.cpp learning/random_util.h)

target_link_libraries(MaxFlowBenchmark Boost::graph)
