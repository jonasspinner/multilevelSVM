#include "results.h"
#include <iomanip>
#include <iostream>

void results::next() { this->cur_iteration += 1; }

void results::setFloat(const std::string &tag, float result) {
    auto [it, inserted] = this->floats.try_emplace(tag, this->cur_iteration + 1, 0);
    if (inserted) {
        this->tag_order.push_back(tag);
    }
    it->second.resize(this->cur_iteration + 1);
    it->second[this->cur_iteration] = result;
}

void results::setString(const std::string &tag, const std::string &result) {
    auto [it, inserted] = this->strings.try_emplace(tag, this->cur_iteration + 1);
    if (inserted) {
        this->tag_order.push_back(tag);
    }
    it->second.resize(this->cur_iteration + 1);
    it->second[this->cur_iteration] = result;
}

void results::print() {
    std::cout << std::setprecision(4) << std::fixed;
    for (const auto &tag : this->tag_order) {
        if (this->floats.find(tag) != this->floats.end()) {
            std::vector<float> &ress = this->floats[tag];
            float average = 0;
            for (const float res : ress) {
                average += res;
            }

            average /= this->cur_iteration + 1;

            std::cout << tag << "\t" << average << std::endl;
        } else {
            std::vector<std::string> &strs = this->strings[tag];
            std::cout << "[" << tag << "]" << std::endl;
            for (size_t i = 0; i < strs.size(); i++) {
                std::cout << "fold " << i << ": " << strs[i] << std::endl;
            }
        }
    }
}
