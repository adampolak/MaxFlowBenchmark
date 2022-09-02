//
// Created by Max Zub on 09/05/2022.
//

#ifndef MAXFLOWBENCHMARK_DISTRIBUTION_H
#define MAXFLOWBENCHMARK_DISTRIBUTION_H

#include <lemon/dimacs.h>
#include <lemon/smart_graph.h>

#include <algorithm>
#include <fstream>
#include <random>
#include <string>

class distribution {
  public:
    distribution(const std::string &dimacs_filename, double sigma) : _capacities(_graph), _sigma(sigma) {
        std::ifstream in(dimacs_filename);
        if (!in) {
            std::cerr << "Cannot open DIMACS file: " << dimacs_filename << std::endl;
            exit(1);
        }
        lemon::readDimacsMax(in, _graph, _capacities, _s, _t);
    }

    const lemon::SmartDigraph& graph() const { return _graph; }
    lemon::SmartDigraph::Node s() const { return _s; }
    lemon::SmartDigraph::Node t() const { return _t; }
    
    template<class Generator> void sample_capacities(
            lemon::SmartDigraph::ArcMap<int64_t> &sampled_capacities,
            Generator &g) const {
        std::normal_distribution<double> distribution(1.0, _sigma);
        for (auto aIt = lemon::SmartDigraph::ArcIt(_graph); aIt != lemon::INVALID; ++aIt)
            sampled_capacities[aIt] = (int64_t)(_capacities[aIt] * std::max(distribution(g), 0.0));
    }

  private:
    lemon::SmartDigraph _graph;
    lemon::SmartDigraph::ArcMap<int64_t> _capacities;
    lemon::SmartDigraph::Node _s, _t;
    double _sigma;
};

#endif //MAXFLOWBENCHMARK_DISTRIBUTION_H
