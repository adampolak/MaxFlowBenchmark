//
// Created by Max Zub on 16/05/2022.
//

#include "dinic_algo.h"


dinic_algo::dinic_algo(Graph& g, Vertex s, Vertex t) {
    this->name = "dinic_algo";
    this->g = &g;
    this->s = s;
    this->t = t;
}

long long dinic_algo::find_flow() {
    Dinic dinic(*g, s, t);
    return 0;
    return dinic.flow();
}