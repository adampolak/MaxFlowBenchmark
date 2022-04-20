//
// Created by Max Zub on 20/04/2022.
//

#ifndef MAXFLOWBENCHMARK_BOYKOV_KOLMOGOROV_H
#define MAXFLOWBENCHMARK_BOYKOV_KOLMOGOROV_H


#include "algorithm.h"

class boykov_kolmogorov: public algorithm {
public:
    boykov_kolmogorov();
    long long find_flow(Graph &g, Traits::vertex_descriptor s, Traits::vertex_descriptor t) override;
};


#endif //MAXFLOWBENCHMARK_BOYKOV_KOLMOGOROV_H
