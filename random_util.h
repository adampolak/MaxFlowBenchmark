//
// Created by Max Zub on 09/05/2022.
//

#ifndef MAXFLOWBENCHMARK_RANDOM_UTIL_H
#define MAXFLOWBENCHMARK_RANDOM_UTIL_H

#include <lemon/smart_graph.h>
#include <random>

class random_util {
public:
    random_util(double X);
    void randomize_capacities(const lemon::SmartDigraph &g, lemon::SmartDigraph::ArcMap<long long> &capacities, const lemon::SmartDigraph::ArcMap<long long> &orig_cap, std::default_random_engine &generator);
private:
    double X;
};

class input_distribution {
public:
    input_distribution(const std::string &dimacs_filename, double sigma);
    const lemon::SmartDigraph& graph() { return _graph; }
    const lemon::SmartDigraph::Node& s() { return _s; }
    const lemon::SmartDigraph::Node& t() { return _t; }
    void sample_capacities(
        lemon::SmartDigraph::ArcMap<int64_t> &sampled_capacities,
        std::default_random_engine &generator);
private:
    lemon::SmartDigraph _graph;
    lemon::SmartDigraph::ArcMap<int64_t> _capacities;
    lemon::SmartDigraph::Node _s, _t;
    double _sigma;
};

#endif //MAXFLOWBENCHMARK_RANDOM_UTIL_H
