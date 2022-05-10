//
// Created by Max Zub on 20/04/2022.
//

#include "push_relabel.h"
#include <boost/graph/push_relabel_max_flow.hpp>
#include <iostream>

push_relabel::push_relabel(Graph& g, Vertex s, Vertex t) {
    this->name = "push_relabel";
    this->g = &g;
    this->s = s;
    this->t = t;
}

long long push_relabel::find_flow() {
    //std::cout << "push_relabel\n";
    return push_relabel_max_flow(*g, s, t);
}