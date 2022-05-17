//
// Created by Max Zub on 20/04/2022.
//

#include "boykov_kolmogorov.h"
#include <boost/graph/boykov_kolmogorov_max_flow.hpp>
#include <iostream>

boykov_kolmogorov::boykov_kolmogorov(Graph& g, Vertex s, Vertex t) {
    this->name = "boykov_kolmogorov";
    this->g = &g;
    this->s = s;
    this->t = t;
}

long long boykov_kolmogorov::find_flow() {
    return boykov_kolmogorov_max_flow(*g, s, t);
}