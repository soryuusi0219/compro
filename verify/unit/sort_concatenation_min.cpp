#include <algorithm>
#include <array>
#include <cassert>
#include <iostream>
#include <numeric>
#include <random>
#include <string>
#include <vector>

#include "library/string/sort_concatenation_min.hpp"

namespace {

template<class Sequence>
Sequence concatenate(const std::vector<Sequence>& sequences) {
  Sequence result;
  for(const auto& sequence : sequences) {
    result.insert(result.end(), sequence.begin(), sequence.end());
  }
  return result;
}

template<class Sequence>
Sequence brute_minimum_concatenation(const std::vector<Sequence>& sequences) {
  std::vector<int> order(sequences.size());
  std::iota(order.begin(), order.end(), 0);
  Sequence best;
  bool initialized = false;
  do {
    Sequence candidate;
    for(int index : order) {
      candidate.insert(candidate.end(), sequences[index].begin(),
                       sequences[index].end());
    }
    if(!initialized || candidate < best) {
      best = std::move(candidate);
      initialized = true;
    }
  } while(std::next_permutation(order.begin(), order.end()));
  return best;
}

template<class Sequence>
void verify(std::vector<Sequence> sequences) {
  const Sequence expected = brute_minimum_concatenation(sequences);
  soryuusi::sort_concatenation_min(sequences);
  assert(concatenate(sequences) == expected);
}

}  // namespace

int main() {
  verify<std::string>({});
  verify<std::string>({""});
  verify<std::string>({"","a"});
  verify<std::string>({"b", "ba", "ab", "aba"});
  verify<std::string>({"9", "91", "90", ""});
  verify<std::vector<int>>({{}, {1}, {1, 0}, {0, 2}, {0}});
  verify<std::vector<long long>>({{10, -1}, {10}, {-1, 10}, {}});
  verify<std::vector<bool>>({{true, false}, {true}, {false, true}, {}});

  std::vector<std::string> equivalent = {"aa", "a", "", "aaa"};
  soryuusi::sort_concatenation_min(equivalent);
  assert(concatenate(equivalent) == "aaaaaa");

  std::vector<std::array<int, 2>> arrays = {{{2, 0}}, {{1, 3}}, {{1, 2}}};
  soryuusi::sort_concatenation_min(arrays);
  assert((arrays ==
          std::vector<std::array<int, 2>>{{{1, 2}}, {{1, 3}}, {{2, 0}}}));

  std::mt19937 rng(20261020);
  for(int trial = 0; trial < 500; ++trial) {
    const int n = rng() % 8;
    std::vector<std::vector<int>> sequences(n);
    for(auto& sequence : sequences) {
      const int length = rng() % 5;
      sequence.resize(length);
      for(int& value : sequence) value = rng() % 4;
    }
    verify(sequences);
  }

  std::cout << "sort_concatenation_min: all tests passed\n";
}
