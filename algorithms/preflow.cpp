//
// Created by Max Zub on 16/05/2022.
//

#include "preflow.h"
#include <lemon/maps.h>

preflow::preflow(
    lemon::SmartDigraph& g,
    lemon::SmartDigraph::ArcMap<long long> *capacity,
    lemon::SmartDigraph::Node s,
    lemon::SmartDigraph::Node t
) {
    this->name = "preflow";

    this->prflw = new lemon::Preflow<lemon::SmartDigraph, lemon::SmartDigraph::ArcMap<long long>> (
        g, *capacity, s, t
    );
}

preflow::~preflow() {
    delete prflw;
}

void preflow::build() {
}

long long preflow::find_flow() {
    prflw->run();
    return prflw->flowValue();
}