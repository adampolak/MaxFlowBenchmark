//
// Created by Max Zub on 16/05/2022.
//

#ifndef MAXFLOWBENCHMARK_PREFLOW_H
#define MAXFLOWBENCHMARK_PREFLOW_H

#include "algorithm.h"
#include <lemon/preflow.h>

class preflow: public algorithm {
public:
    preflow(
        Graph& g, Vertex s, Vertex t
    );
    ~preflow();
    long long find_flow() override;
private:
    lemon::SmartDigraph gg;
    lemon::SmartDigraph::ArcMap<long long> *caps;
    lemon::Preflow<
        lemon::SmartDigraph,
        lemon::SmartDigraph::ArcMap<long long>
    > *prflw;
    std::vector<lemon::SmartDigraph::Node> node_mapping;
    Graph* g;
};


#endif //MAXFLOWBENCHMARK_PREFLOW_H
