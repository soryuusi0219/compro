// https://github.com/soryuusi0219/compro/blob/main/library/string/trie.hpp
#pragma once
#include <algorithm>
#include <cassert>
#include <iostream>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

namespace soryuusi {

using namespace std;

template<class C, C MIN_CHARCTER = 'a', C MAX_CHARCTER = 'z'>
struct Trie{
  static constexpr int ALPHA = MAX_CHARCTER - MIN_CHARCTER + 1;
  using VC = conditional_t<is_same_v<C, char>, string, vector<C>>;
  struct Node {
    int next[ALPHA];
    int p;
    C pch;
    int z, sz;
    Node(int _p=-1, C _pch=0): p(_p), pch(_pch), z(0), sz(0){
      fill(next, next+ALPHA, -1);
    }
  };
  void show_node(int idx, const string& indent, bool is_last) const {
    if (idx == 0) cout << indent << "(root)" << " [sub=" << nodes[idx].sz << "]\n";
    else cout << indent << (is_last ? "└── " : "├── ") << nodes[idx].pch << " [end=" << nodes[idx].z << ", sub=" << nodes[idx].sz << "]\n";
    vector<pair<C,int>> childs;
    for (int c = 0; c < ALPHA; c++) {
      int v = nodes[idx].next[c];
      if (v != -1) childs.emplace_back(MIN_CHARCTER+c, v);
    }
    for (int i = 0; i < (int)childs.size(); i++) {
      bool last = (i + 1 == (int)childs.size());
      string next_indent = indent + (idx==0 ? "" : (is_last ? "    " : "│   "));
      show_node(childs[i].second, next_indent, last);
    }
  }
  vector<Node> nodes;
  Trie() {
    nodes.emplace_back(-1, 0);
  }
  void insert(const VC& s) {
    int p = 0;
    auto it = s.begin();
    const auto end = s.end();
    for (; it != end; ++it) {
      ++nodes[p].sz;
      const int child = nodes[p].next[*it-MIN_CHARCTER];
      if (child == -1) break;
      p = child;
    }
    if (it == end) ++nodes[p].sz;
    // After the first missing edge, the entire remaining path is new.
    for (; it != end; ++it) {
      const C ch = *it;
      const int child = nodes.size();
      nodes[p].next[ch-MIN_CHARCTER] = child;
      nodes.emplace_back(p, ch);
      nodes.back().sz = 1;
      p = child;
    }
    ++nodes[p].z;
  } 
  int count(const VC& s) const {
    const int p = node_index_of(s);
    return p == -1 ? 0 : nodes[p].z;
  }
  int prefix_count(const VC& s) const {
    const int p = node_index_of(s);
    return p == -1 ? 0 : nodes[p].sz;
  }
  int size() const {
    return nodes[0].sz;
  }
  VC kth(int k) const {
    assert(0 <= k && k < size());
    VC res;
    int p = 0;
    while (true) {
      if (k < nodes[p].z) return res;
      k -= nodes[p].z;
      for (int c = 0; c < ALPHA; c++) {
        int nx = nodes[p].next[c];
        if (nx == -1) continue;
        if (k < nodes[nx].sz) {
          res.push_back(c + MIN_CHARCTER);
          p = nx;
          break;
        }
        k -= nodes[nx].sz;
      }
    }
  }
  VC min_string() const {return kth(0);}
  VC max_string() const {return kth(size()-1);}
  int longest_prefix(const VC& s) const {
    int p = 0;
    int best = 0, cur = 0;
    for (C ch : s) {
      int i = ch - MIN_CHARCTER;
      int nx = nodes[p].next[i];
      if (nx == -1 || nodes[nx].sz == 0) break;
      cur++;
      p = nx;
      if (nodes[p].z > 0) best = cur;
    }
    return best;
  }
  int lcp(const VC& s) const {
    int p = 0;
    int cur = 0;
    for (C ch : s) {
      int i = ch - MIN_CHARCTER;
      int nx = nodes[p].next[i];
      if (nx == -1 || nodes[nx].sz == 0) break;
      cur++;
      p = nx;
    }
    return cur;
  }
  int node_index_of(const VC& s) const {
    int p = 0;
    for (C ch : s) {
      p = nodes[p].next[ch-MIN_CHARCTER];
      if (p == -1) return -1;
    }
    return p;
  }
  void show() const {
    show_node(0, "", true);
  }
};

}  // namespace soryuusi
