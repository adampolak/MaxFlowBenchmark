//
// Created by Max Zub on 20/04/2022.
//

#ifndef MAXFLOWBENCHMARK_ALGORITHM_H
#define MAXFLOWBENCHMARK_ALGORITHM_H


#include <string>
#include <boost/config.hpp>
#include <boost/graph/adjacency_list.hpp>

using namespace boost;

typedef adjacency_list_traits < vecS, vecS, directedS > Traits;
typedef adjacency_list < vecS, vecS, directedS,
property < vertex_name_t, std::string,
property < vertex_index_t, long,
property < vertex_color_t, boost::default_color_type,
property < vertex_distance_t, long,
property < vertex_predecessor_t, Traits::edge_descriptor > > > > >,

property < edge_capacity_t, long,
property < edge_residual_capacity_t, long,
property < edge_reverse_t, Traits::edge_descriptor > > > > Graph;


class algorithm {
public:
    virtual long long find_flow(Graph& g, Traits::vertex_descriptor s, Traits::vertex_descriptor t) = 0;
    std::string name;
};


#endif //MAXFLOWBENCHMARK_ALGORITHM_H
