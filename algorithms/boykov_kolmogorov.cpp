//
// Created by Max Zub on 20/04/2022.
//

#include "boykov_kolmogorov.h"
#include <boost/graph/boykov_kolmogorov_max_flow.hpp>
#include <iostream>

boykov_kolmogorov::boykov_kolmogorov() {
    this->name = "boykov_kolmogorov";
}

long long boykov_kolmogorov::find_flow(Graph &g, Traits::vertex_descriptor s, Traits::vertex_descriptor t) {
    //std::cout << "boykov\n";
    return boykov_kolmogorov_max_flow(g, s, t);
}