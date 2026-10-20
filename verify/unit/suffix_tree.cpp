#include <algorithm>
#include <cassert>
#include <iostream>
#include <numeric>
#include <random>
#include <set>
#include <string>
#include <vector>

#include "library/string/suffix_tree.hpp"

namespace {

int naive_lcp(const std::string& s, int i, int j) {
  int length = 0;
  while (i + length < static_cast<int>(s.size()) &&
         j + length < static_cast<int>(s.size()) &&
         s[i + length] == s[j + length]) {
    ++length;
  }
  return length;
}

int naive_count(const std::string& s, const std::string& pattern) {
  int count = 0;
  for (int i = 0; i + static_cast<int>(pattern.size()) <=
                  static_cast<int>(s.size());
       ++i) {
    count += s.compare(i, pattern.size(), pattern) == 0;
  }
  return count;
}

int naive_matching_suffix_length(const std::string& s,
                                 const std::string& text, int right) {
  for (int length = std::min<int>(s.size(), right); length > 0; --length) {
    if (s.find(text.substr(right - length, length)) != std::string::npos) {
      return length;
    }
  }
  return 0;
}

template <bool UseMap>
void verify_tree(const std::string& s, std::mt19937& rng) {
  using Tree = soryuusi::SuffixTree<char, 'a', 'c', UseMap>;
  Tree tree(s);
  const int n = static_cast<int>(s.size());

  std::vector<int> expected_sa(n);
  std::iota(expected_sa.begin(), expected_sa.end(), 0);
  std::sort(expected_sa.begin(), expected_sa.end(), [&](int i, int j) {
    return s.substr(i) < s.substr(j);
  });
  assert(tree.suffix_array() == expected_sa);

  std::vector<int> expected_isa(n);
  for (int i = 0; i < n; ++i) expected_isa[expected_sa[i]] = i;
  assert(tree.suffix_array_inv() == expected_isa);

  std::vector<int> expected_lcp(std::max(0, n - 1));
  for (int i = 0; i + 1 < n; ++i) {
    expected_lcp[i] = naive_lcp(s, expected_sa[i], expected_sa[i + 1]);
  }
  assert(tree.lcp_array() == expected_lcp);

  std::set<std::string> substrings;
  for (int left = 0; left < n; ++left) {
    for (int length = 1; left + length <= n; ++length) {
      substrings.insert(s.substr(left, length));
    }
  }
  assert(tree.distinct_substrings() ==
         static_cast<long long>(substrings.size()));

  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < n; ++j) {
      assert(tree.suffix_LCP(i, j) == naive_lcp(s, i, j));
    }
  }

  const auto& nodes = tree.get_nodes();
  assert(!nodes.empty());
  for (int v = 0; v < static_cast<int>(nodes.size()); ++v) {
    assert(&tree.get_node(v) == &nodes[v]);
    assert(nodes[v].len() == nodes[v].r - nodes[v].l);
    assert(0 <= nodes[v].l && nodes[v].l <= nodes[v].r && nodes[v].r <= n);
    assert(nodes[v].depth >= 0 && nodes[v].count >= 0);
  }

  for (int length = 1; length <= n + 2; ++length) {
    for (int trial = 0; trial < 5; ++trial) {
      std::string pattern(length, 'a');
      for (char& c : pattern) c = static_cast<char>('a' + rng() % 3);
      assert(tree.count_occurrences(pattern) == naive_count(s, pattern));
    }
  }
  for (const auto& pattern : substrings) {
    assert(tree.count_occurrences(pattern) == naive_count(s, pattern));
  }

  std::string text(2 * n + 7, 'a');
  for (char& c : text) c = static_cast<char>('a' + rng() % 3);
  const auto states = tree.matching_states(text);
  assert(states.size() == text.size());
  for (int i = 0; i < static_cast<int>(text.size()); ++i) {
    const auto& state = states[i];
    const int length = naive_matching_suffix_length(s, text, i + 1);
    assert(tree.state_len(state) == length);
    const int left = tree.state_left(state);
    const int right = tree.state_right(state);
    assert(right - left == length);
    assert(0 <= left && left <= right && right <= n);
    assert(s.substr(left, length) == text.substr(i + 1 - length, length));
    if (length > 0) {
      assert(tree.state_count(state) ==
             naive_count(s, text.substr(i + 1 - length, length)));
    }
  }

  const auto [s_left, t_left, length] = tree.longest_common_substring(text);
  int expected_length = 0;
  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < static_cast<int>(text.size()); ++j) {
      int common = 0;
      while (i + common < n && j + common < static_cast<int>(text.size()) &&
             s[i + common] == text[j + common]) {
        ++common;
      }
      expected_length = std::max(expected_length, common);
    }
  }
  assert(length == expected_length);
  assert(0 <= s_left && s_left + length <= n);
  assert(0 <= t_left && t_left + length <= static_cast<int>(text.size()));
  assert(s.substr(s_left, length) == text.substr(t_left, length));
}

}  // namespace

int main() {
  soryuusi::SuffixTree<> empty;
  assert(empty.suffix_array().empty());
  assert(empty.suffix_array_inv().empty());
  assert(empty.lcp_array().empty());
  assert(empty.distinct_substrings() == 0);
  assert(empty.count_occurrences(std::string("a")) == 0);
  const auto [a, b, length] =
      empty.longest_common_substring(std::string("abc"));
  assert(a == 0 && b == 0 && length == 0);

  std::mt19937 rng(20261020);
  const std::vector<std::string> fixed = {
      "a", "aaaa", "ababa", "bacaba", "abcabcabc", "ccabacabbac"};
  for (const auto& s : fixed) {
    verify_tree<false>(s, rng);
    verify_tree<true>(s, rng);
  }
  for (int n = 1; n <= 32; ++n) {
    for (int trial = 0; trial < 20; ++trial) {
      std::string s(n, 'a');
      for (char& c : s) c = static_cast<char>('a' + rng() % 3);
      verify_tree<false>(s, rng);
      if (trial < 3) verify_tree<true>(s, rng);
    }
  }

  std::cout << "suffix_tree: all tests passed\n";
}
