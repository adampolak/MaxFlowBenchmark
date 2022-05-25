//
// Created by Max Zub on 25/05/2022.
//

#ifndef MAXFLOWBENCHMARK_ALGOS_H
#define MAXFLOWBENCHMARK_ALGOS_H


#include <string>

class algos {
public:
    void run(
        const std::string& graph_filename,
        const std::string& preprocessed_flows_filename,
        const std::string& results_file,
        int test_samples,
        double X
    );
};


#endif //MAXFLOWBENCHMARK_ALGOS_H
