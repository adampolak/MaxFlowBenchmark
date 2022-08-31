#include <iostream>
#include "learning/learning.h"
#include "algos.h"
#include "distribution.h"

/**
 *    usage:
 *    1. run algorithms benchmark:
 *       algo graph_filename preprocessed_flows_filename results_file test_samples_number X algorithms_bitmask
 *
 *    2. run learning
 *       learn graph_filename n_samples X
 *
 */

int main(int argc, const char* argv[]) {
    if (argc < 2) {
        std::cerr << "incorrect number of arguments" << std::endl;
        return 1;
    }

    std::string command = argv[1];

    if (command == "learn") {
        if (argc != 6) {
            std::cerr << "incorrect number of arguments" << std::endl;
            return 1;
        }

        int n_samples = atoi(argv[2]);
        std::string dimacs_filename = argv[3];
        std::stringstream buffer(argv[4]);
        double sigma;
        buffer >> sigma;
        std::string predictions_filename = argv[5];

        distribution d(dimacs_filename, sigma);
        lemon::SmartDigraph::ArcMap<int64_t> predictions(d.graph());
        learn_predictions(d, n_samples, predictions);
        store_predictions(d.graph(), predictions, predictions_filename);
        return 0;
    } else if (command == "algo") {
        // if (argc != 8) {
        //     std::cerr << "incorrect number of arguments" << std::endl;
        //     return 1;
        // }

        int n_samples = atoi(argv[2]);
        std::string dimacs_filename = argv[3];
        std::stringstream buffer(argv[4]);
        double sigma;
        buffer >> sigma;
        std::string predictions_filename = argv[5];
        
        distribution d(dimacs_filename, sigma);
        lemon::SmartDigraph::ArcMap<int64_t> predictions(d.graph());
        load_predictions(d.graph(), predictions_filename, predictions);
        verify_predictions(d.graph(), d.s(), d.t(), predictions);

        // std::string dimacs_filename = argv[2];
        // std::string preprocessed_flows_filename = argv[3];
        // std::string results_file = argv[4];
        // int test_samples = atoi(argv[5]);
        // std::stringstream strIn(argv[6]);
        // int algorithms_bitmask = atoi(argv[7]);
        // std::cout << algorithms_bitmask << std::endl;
        // double sigma;
        // strIn >> sigma;
        // distribution d(dimacs_filename, sigma);
        // algos alg;
        // alg.run(d, preprocessed_flows_filename, results_file, test_samples, algorithms_bitmask);
        // return 0;
    } else {
        std::cerr << "incorrect first argument, should be learn or algo" << std::endl;
        return 1;
    }
}
