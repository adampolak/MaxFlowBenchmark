//
// Created by Max Zub on 20/04/2022.
//

#ifndef MAXFLOWBENCHMARK_BOYKOV_KOLMOGOROV_H
#define MAXFLOWBENCHMARK_BOYKOV_KOLMOGOROV_H


#include "algorithm.h"

class boykov_kolmogorov: public algorithm {
public:
    boykov_kolmogorov(Graph& g, Vertex s, Vertex t);
    long long find_flow() override;
private:
    Graph *g;
    Vertex s, t;
};


#endif //MAXFLOWBENCHMARK_BOYKOV_KOLMOGOROV_H
