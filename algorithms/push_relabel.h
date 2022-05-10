//
// Created by Max Zub on 20/04/2022.
//

#ifndef MAXFLOWBENCHMARK_PUSH_RELABEL_H
#define MAXFLOWBENCHMARK_PUSH_RELABEL_H


#include "algorithm.h"

class push_relabel: public algorithm {
public:
    push_relabel(Graph& g, Vertex s, Vertex t);
    long long find_flow() override;
private:
    Graph *g;
    Vertex s, t;
};


#endif //MAXFLOWBENCHMARK_PUSH_RELABEL_H
