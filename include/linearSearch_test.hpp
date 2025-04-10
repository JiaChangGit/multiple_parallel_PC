#ifndef _CLASSIFIERS_LINEARSEARCH_HPP_
#define _CLASSIFIERS_LINEARSEARCH_HPP_

#include <fstream>

#include "data_structure.hpp"

class LinearSearch {
 public:
  // explicit LinearSearch() { results.resize(0); };
  // explicit LinearSearch(int _packetNum) { results.resize(_packetNum); };

  int inline search(const std::vector<Rule> &rules, const Packet &packet) {
    int counter = 0;

    for (const auto &rule : rules) {
      if (rule.isMatch(packet)) {
        // std::cout << "match: " << rule.priority << "\n";
        break;
      }
      ++counter;
    }
    return rules[counter].priority;
  };

 private:
  // std::vector<int> results;
};
#endif
