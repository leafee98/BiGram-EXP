#include <iostream>
#include <fstream>
#include <regex>
#include <string>
#include <map>
#include <vector>

#include "utils.hpp"


std::vector<std::string> split_words(std::string line) {
    static std::regex journal_number(R"(^\d{8}-\d{2}-\d{3}-\d{3}/m\s+)");
    static std::regex words(R"(([^\s]+)/[a-zA-Z]+?)");

    // remove the journal_number in front of each line
    line = std::regex_replace(line, journal_number, "");

    std::vector<std::string> word_list;
    std::smatch word_match;
    while (std::regex_search(line, word_match, words)) {
        // std::cout << "found word: " << word_match[1] << std::endl;

        word_list.emplace_back(word_match[1]);
        line = word_match.suffix();
    }

    return word_list;
}


int main(int argc, char ** argv) {
    if (argc != 2) {
        std::cout << "usage: " << argv[0] << "<data file>" << std::endl;
        return 1;
    }

    std::map<std::string, std::string> next_count;

    std::ifstream datafile(argv[1], std::ios::in);

    std::vector<std::vector<std::string>> full_data;
    for (std::string line; std::getline(datafile, line); ) {
        std::vector<std::string> word_list = split_words(line);
        full_data.emplace_back(word_list);
    }

    bigram_tool b_tool(full_data);

    std::cout << b_tool.export_str() << std::endl;

    return 0;
}