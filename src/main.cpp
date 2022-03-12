#include <cstdlib>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include "utils.hpp"

bool DEBUG = false;

int main(int argc, char ** argv) {
    if (argc != 2) {
        std::cout << "usage: " << argv[0] << " <model file>" << std::endl;
        return 1;
    }

    DEBUG = std::getenv("BIGRAM_DEBUG") != NULL;

    std::cout << "loading model..." << std::endl;

    std::ifstream infile(argv[1], std::ios::in);
    std::stringstream ss;
    ss << infile.rdbuf();
    bigram_tool bt = bigram_tool::import_str(ss.str());

    std::cout << "succeed!" << std::endl;

    std::string s;
    std::cout << "> ";
    std::cout.flush();
    while (std::getline(std::cin, s)) {
        std::vector<std::vector<unsigned>> sequences = bt.get_possible_sequences(s);

        for (const std::vector<unsigned> & seq : sequences) {
            if (DEBUG) {
                std::cout << "================================" << std::endl;
                std::cout << "DEBUG: sequence is ";
                for (size_t i = 0; i < seq.size(); i++) {
                    if (i != 0)
                        std::cout << ' ';
                    std::cout << seq[i];
                }
                std::cout << std::endl;
            }

            double possibility = bt.calc_possibility(seq, DEBUG);
            std::string formated_str = bt.sequence_to_string(seq);

            std::cout << formated_str << ':' << possibility << std::endl;
        }
        std::cout << "> ";
        std::cout.flush();
    }
    return 0;
}