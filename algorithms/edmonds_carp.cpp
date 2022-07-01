//
// Created by Max Zub on 25/05/2022.
//

#include "edmonds_carp.h"

edmonds_carp::edmonds_carp(
    lemon::SmartDigraph& g,
    lemon::SmartDigraph::ArcMap<long long> capacity,
    lemon::SmartDigraph::Node s,
    lemon::SmartDigraph::Node t
) {
    this->name = "edmonds_karp";

    this->edm_karp = new lemon::EdmondsKarp<lemon::SmartDigraph, lemon::SmartDigraph::ArcMap<long long>> (
        g, capacity, s, t
    );
}

edmonds_carp::~edmonds_carp() {
    delete edm_karp;
}

void edmonds_carp::build() {
}

long long edmonds_carp::find_flow() {
    edm_karp->run();
    return edm_karp->flowValue();
}