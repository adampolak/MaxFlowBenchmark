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
        lemon::SmartDigraph& g,
        lemon::SmartDigraph::ArcMap<long long> *capacity,
        lemon::SmartDigraph::Node s,
        lemon::SmartDigraph::Node t
    );
    ~preflow();
    long long find_flow() override;
    void build() override;
private:
    lemon::Preflow<
        lemon::SmartDigraph,
        lemon::SmartDigraph::ArcMap<long long>
    > *prflw;
};


#endif //MAXFLOWBENCHMARK_PREFLOW_H
