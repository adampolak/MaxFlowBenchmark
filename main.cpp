#include <iostream>
#include "learning/learning.h"
#include "algos.h"

/**
 *    usage:
 *    1. run algorithms benchmark:
 *       algo graph_filename preprocessed_flows_filename results_file test_samples_number X algorithms_bitmask
 *
 *    2. run learning
 *       learn graph_filename n_samples X
 *
 */

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "incorrect args!";
        return 1;
    }

    std::string type = argv[1];

    if (type == "learn") {
        if (argc < 5) {
            std::cerr << "incorrect args!";
            return 1;
        }
        std::string graph_filename = argv[2];
        int n_samples = atoi(argv[3]);
        std::stringstream strIn(argv[4]);
        double X;
        strIn >> X;
        learning lrn;
        lrn.start(graph_filename, n_samples, X);
        return 0;
    }

    if (type == "algo") {
        if (argc < 8) {
            std::cerr << "incorrect args!";
            return 1;
        }
        std::string graph_filename = argv[2];
        std::string preprocessed_flows_filename = argv[3];
        std::string results_file = argv[4];
        int test_samples = atoi(argv[5]);
        std::stringstream strIn(argv[6]);
        int algorithms_bitmask = atoi(argv[7]);
        std::cout << algorithms_bitmask << std::endl;
        double X;
        strIn >> X;
        algos alg;
        alg.run(graph_filename, preprocessed_flows_filename, results_file, test_samples, algorithms_bitmask, X);
        return 0;
    }

    std::cerr << "incorrect args!";

    return 1;
}
