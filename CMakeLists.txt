cmake_minimum_required(VERSION 3.21)
project(MaxFlowBenchmark)

set(CMAKE_CXX_STANDARD 14)

find_package(Boost COMPONENTS graph REQUIRED)

add_executable(MaxFlowBenchmark main.cpp algorithms/algorithm.cpp algorithms/algorithm.h algorithms/boykov_kolmogorov.cpp algorithms/boykov_kolmogorov.h algorithms/push_relabel.cpp algorithms/push_relabel.h algorithms/learning_augmented.cpp algorithms/learning_augmented.h)

target_link_libraries(MaxFlowBenchmark Boost::graph)
