//
// Created by Max Zub on 09/05/2022.
//

#include "random_util.h"

#include <lemon/dimacs.h>

#include <fstream>


random_util::random_util(double X): X(X) {};


void random_util::randomize_capacities(const lemon::SmartDigraph &g,
                                       lemon::SmartDigraph::ArcMap<long long> &capacities,
                                       const  lemon::SmartDigraph::ArcMap<long long> &orig_cap,
                                       std::default_random_engine &generator) {



    auto aIt = lemon::SmartDigraph::ArcIt(g);

    for (; aIt != lemon::INVALID; ++aIt) {
        std::normal_distribution<double> distribution(orig_cap[aIt], double(orig_cap[aIt])*X);
        long long gen_cap = std::max((long long)(distribution(generator)), 0ll);
        capacities[aIt] = gen_cap;
    }
}

input_distribution::input_distribution(const std::string &dimacs_filename, double sigma) : _capacities(_graph), _sigma(sigma) {
    std::ifstream in(dimacs_filename);
    if (!in) {
        std::cerr << "Cannot open DIMACS file: " << dimacs_filename << std::endl;
        exit(1);
    }
    lemon::readDimacsMax(in, _graph, _capacities, _s, _t);
}

void input_distribution::sample_capacities(
        lemon::SmartDigraph::ArcMap<int64_t> &sampled_capacities,
        std::default_random_engine &generator) {
    std::normal_distribution<double> distribution(1.0, _sigma);
    for (auto aIt = lemon::SmartDigraph::ArcIt(_graph); aIt != lemon::INVALID; ++aIt)
        sampled_capacities[aIt] = (int64_t)(_capacities[aIt] * std::max(distribution(generator), 0.0));
}
