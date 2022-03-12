#ifndef UTILS_HPP
#define UTILS_HPP

#include <map>
#include <set>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <cassert>
#include <functional>

class bigram_tool {
private:
    // 0 is placeholder, 1 is BEGIN, 2 is END
    std::map<std::string, unsigned> word_to_num;
    std::vector<std::string> num_to_word;

    // bi_matrix[A][B] means the count of B occurres after A.
    // bi_matrix[A][0] means the count of A occurres.
    // std::vector<std::vector<std::string>> bi_matrix;
    std::map<unsigned, std::map<unsigned, unsigned>> bi_matrix;

    static std::map<std::string, unsigned> init_word_to_num(const std::vector<std::string> & ntw) {
        std::map<std::string, unsigned> wtn;
        for (size_t i = 3; i < ntw.size(); i++)
            wtn[ntw[i]] = i;
        return wtn;
    }

    static std::map<unsigned, std::map<unsigned, unsigned>>  init_bi_matrix(
        const std::vector<std::vector<std::string>> & word_data,
        std::map<std::string, unsigned> wtn)
    {
        std::map<unsigned, std::map<unsigned, unsigned>> bi_matrix;
        std::function<void(unsigned, unsigned)> bi_matrix_counter = [&bi_matrix](unsigned a, unsigned b) {
            if (bi_matrix.count(a)) {
                if (bi_matrix[a].count(b)) {
                    bi_matrix[a][b] += 1;
                } else {
                    bi_matrix[a][b] = 1;
                }
            } else {
                std::map<unsigned, unsigned> tmp;
                tmp[b] = 1;
                bi_matrix[a] = tmp;
            }
        };

         for (const std::vector<std::string> & line_words : word_data) {
            if (line_words.size() == 0)
                continue;

            unsigned pre = wtn[line_words[0]];
            bi_matrix_counter(1, pre);

            for (unsigned i = 1; i < line_words.size(); i++) {
                unsigned num = wtn[line_words[i]];
                bi_matrix_counter(pre, num);

                pre = num;
            }

            bi_matrix_counter(pre, 2);
        }

        // init the bi_matrix[A][0], which means the number of times of the A appears
        for (std::map<unsigned, std::map<unsigned, unsigned>>::iterator i = bi_matrix.begin();
                i != bi_matrix.end();
                i++)
        {
            unsigned count_sum = 0;
            for (std::map<unsigned, unsigned>::iterator j = i->second.begin();
                    j!= i->second.end();
                    j++)
            {
                count_sum += j->second;
            }

            // assert no such element like bi_matrix[A][0], the place to store the number of times of the A appears
            assert(i->second.count(0) == 0);
            i->second[0] = count_sum;
        }
        return bi_matrix;
    }

    void get_possible_sequences(
            std::vector<std::vector<unsigned>> & result,
            std::vector<unsigned> tmp,
            const std::string & s)
    {
        for (size_t i = s.size(); i > 0; i--) {
            const std::string & sub = s.substr(0, i);
            if (this->word_to_num.count(sub)) {
                tmp.emplace_back(this->word_to_num[sub]);

                if (i < s.size()) {
                    get_possible_sequences(result, tmp, s.substr(i));
                } else {
                    result.emplace_back(tmp);
                }

                tmp.erase(tmp.end() - 1);
            }
        }
    }

    bigram_tool(std::vector<std::string> num_to_word, std::map<unsigned, std::map<unsigned, unsigned>> matrix) {
        this->num_to_word = num_to_word;
        this->word_to_num = bigram_tool::init_word_to_num(this->num_to_word);

        this->bi_matrix = matrix;
    }


public:
    /*********
     * the word_data is many lines of words
     */
    bigram_tool(const std::vector<std::vector<std::string>> & word_data) {
        // build the map between word and num
        this->num_to_word = std::vector<std::string>{"#", "#BEGIN", "#END"};

        std::set<std::string> s;
        for (const std::vector<std::string> & line_words : word_data) {
            for (const std::string & wd : line_words) {
                s.emplace(wd);
            }
        }

        this->num_to_word.insert(this->num_to_word.end(), s.begin(), s.end());
        this->word_to_num = bigram_tool::init_word_to_num(this->num_to_word);
        this->bi_matrix = bigram_tool::init_bi_matrix(word_data, this->word_to_num);
    }

    std::vector<std::vector<unsigned>> get_possible_sequences(const std::string & s) {
        std::vector<std::vector<unsigned>> result;
        std::vector<unsigned> tmp;
        this->get_possible_sequences(result, tmp, s);
        return result;
    }

    double calc_possibility(const std::vector<unsigned> & v, bool debug = false) {
        double result = 1.0;

        unsigned pre = 1;           // the #BEGIN
        for (unsigned x : v) {
            result *= double(this->bi_matrix[pre][x]) / this->bi_matrix[pre][0];

            if (debug) {
                std::cout << "DEBUG: " << "pre-x: " << pre << "-" << x
                        << "(" << this->num_to_word[pre] << ' ' << this->num_to_word[x] << ")"
                        << "; P(x|pre): " << this->bi_matrix[pre][x] << "/" << this->bi_matrix[pre][0] << std::endl;
            }
            pre = x;
        }

        unsigned x = 2;             // the #END
        result *= double(this->bi_matrix[pre][x]) / this->bi_matrix[pre][0];
        if (debug) {
            std::cout << "DEBUG: " << "pre-x: " << pre << "-" << x
                    << "(" << this->num_to_word[pre] << ' ' << this->num_to_word[x] << ")"
                    << "; P(x|pre): " << this->bi_matrix[pre][x] << "/" << this->bi_matrix[pre][0] << std::endl;
        }

        return result;
    }

    std::string sequence_to_string(const std::vector<unsigned> & v) {
        if (v.size() == 0)
            return "";

        std::stringstream ss;
        ss << this->num_to_word[v[0]];
        for (size_t i = 1; i < v.size(); i++) {
            ss << ' ' << this->num_to_word[v[i]];
        }

        return ss.str();
    }

    /*********
     * export num_to_word and matrix as a big string
     */
    std::string export_str() {
        std::stringstream ss;
        ss << this->num_to_word.size() << std::endl;
        for (const std::string & s : this->num_to_word) {
            ss << s << std::endl;
        }

        for (std::map<unsigned, std::map<unsigned, unsigned>>::iterator i = this->bi_matrix.begin();
                i != this->bi_matrix.end();
                i++)
        {
            for (std::map<unsigned, unsigned>::iterator j = i->second.begin(); j != i->second.end(); j++) {
                ss << i->first << ' ' << j->first << ' ' << j->second << std::endl;
            }
        }

        return ss.str();
    }

    /*********
     * import data from a string, to build a bigram_tool object.
     * the string should come from export_str()
     */
    static bigram_tool import_str(const std::string & s) {
        std::stringstream ss(s);
        unsigned len;
        ss >> len;

        std::vector<std::string> num_to_word(len);
        std::map<unsigned, std::map<unsigned, unsigned>> matrix;

        for (unsigned i = 0; i < len; i++) {
            ss >> num_to_word[i];
        }

        unsigned a, b, count;
        while (ss >> a >> b >> count) {
            if (matrix.count(a)) {
                matrix[a][b] = count;
            } else {
                matrix[a] = std::map<unsigned, unsigned>();
                matrix[a][b] = count;
            }
        }

        return bigram_tool(num_to_word, matrix);
    }

};

#endif