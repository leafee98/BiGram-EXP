#include <cstdlib>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include "utils.hpp"

bool DEBUG = false;

struct participle_item {
    std::string str;
    double possibility;

    bool operator<(const participle_item & p) {
        return this->possibility < p.possibility;
    }
};

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
        std::vector<participle_item> participle_result;
        participle_result.reserve(sequences.size());

        for (const std::vector<unsigned> & seq : sequences) {
            std::string formated_str = bt.sequence_to_string(seq);

            if (DEBUG) {
                std::cout << "================================" << std::endl;
                std::cout << "DEBUG: participle is " << formated_str << std::endl;
                std::cout << "DEBUG: sequence is ";
                for (size_t i = 0; i < seq.size(); i++) {
                    if (i != 0)
                        std::cout << ' ';
                    std::cout << seq[i];
                }
                std::cout << std::endl;
            }

            double possibility = bt.calc_possibility(seq, true, DEBUG);

            participle_result.emplace_back(participle_item{formated_str, possibility});
        }

        std::sort(participle_result.begin(), participle_result.end());

        for (size_t i = 0; i < participle_result.size(); i++) {
            std::cout << participle_result[i].possibility << '\t' << participle_result[i].str << std::endl;
        }

        std::cout << "> ";
        std::cout.flush();
    }
    return 0;
}