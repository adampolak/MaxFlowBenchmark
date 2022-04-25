//
// Created by Max Zub on 20/04/2022.
//

#include "push_relabel.h"
#include <boost/graph/push_relabel_max_flow.hpp>
#include <iostream>

push_relabel::push_relabel() {
    this->name = "push_relabel";
}

long long push_relabel::find_flow(Graph &g, Vertex s, Vertex t) {
    //std::cout << "push_relabel\n";
    return push_relabel_max_flow(g, s, t);
}