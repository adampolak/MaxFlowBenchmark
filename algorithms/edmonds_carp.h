//
// Created by Max Zub on 25/05/2022.
//

#ifndef MAXFLOWBENCHMARK_EDMONDS_CARP_H
#define MAXFLOWBENCHMARK_EDMONDS_CARP_H

#include "algorithm.h"
#include <lemon/edmonds_karp.h>

class edmonds_carp: public algorithm {
public:
    edmonds_carp(
        lemon::SmartDigraph& g,
        lemon::SmartDigraph::ArcMap<long long> capacity,
        lemon::SmartDigraph::Node s,
        lemon::SmartDigraph::Node t
    );
    ~edmonds_carp();
    long long find_flow() override;
    void build() override;
private:
    lemon::EdmondsKarp<
        lemon::SmartDigraph,
        lemon::SmartDigraph::ArcMap<long long>
    > *edm_karp;
};


#endif //MAXFLOWBENCHMARK_EDMONDS_CARP_H
