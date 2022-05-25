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
        Graph& g, Vertex s, Vertex t
    );
    ~edmonds_carp();
    long long find_flow() override;
private:
    lemon::ListDigraph gg;
    lemon::ListDigraph::ArcMap<long long> *caps;
    lemon::EdmondsKarp<
        lemon::ListDigraph,
        lemon::ListDigraph::ArcMap<long long>
    > *edm_karp;
};


#endif //MAXFLOWBENCHMARK_EDMONDS_CARP_H
