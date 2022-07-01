//
// Created by Max Zub on 20/04/2022.
//

#ifndef MAXFLOWBENCHMARK_ALGORITHM_H
#define MAXFLOWBENCHMARK_ALGORITHM_H


#include <string>
#include <queue>
#include <iostream>
#include <lemon/list_graph.h>
#include <lemon/smart_graph.h>
#include <lemon/static_graph.h>


class algorithm {
public:
    double time_preprocess;
    virtual long long find_flow() = 0;
    virtual void build() = 0;
    std::string name;
};


#endif //MAXFLOWBENCHMARK_ALGORITHM_H
