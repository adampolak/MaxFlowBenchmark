//
// Created by Max Zub on 16/05/2022.
//

#ifndef MAXFLOWBENCHMARK_DINIC_ALGO_H
#define MAXFLOWBENCHMARK_DINIC_ALGO_H

#include "algorithm.h"

class dinic_algo: public algorithm {
public:
    dinic_algo(Graph& g, Vertex s, Vertex t);
    long long find_flow() override;
private:
    Graph *g;
    Vertex s, t;
};


#endif //MAXFLOWBENCHMARK_DINIC_ALGO_H
