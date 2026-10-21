// https://github.com/soryuusi0219/compro/blob/main/library/graph/dynamic_connectivity_offline.hpp
#pragma once
#include <algorithm>
#include <cassert>
#include <functional>
#include <map>
#include <stack>
#include <tuple>
#include <utility>
#include <vector>

#include "library/data_structure/dsu_undo.hpp"
#include "library/graph/static_graph.hpp"

namespace soryuusi {

using namespace std;

struct dynamic_connectivity_offline{
  dsu_undo uf;
  using edge = pair<int,int>;
  int n, q;
  int sz, szlog;
  static_graph<int> seg;
  map<edge, int> cnt, appear;
  vector<tuple<int,int, edge>> pend;

  dynamic_connectivity_offline(int n_, int q_) : uf(n_), n(n_), q(q_){
    sz = 1, szlog = 0; while(sz < q) sz <<= 1, szlog++;
    seg = static_graph<int>(sz << 1);
  }

  void add_edge(int idx, int u, int v){
    auto e = minmax(u,v);
    if(!(cnt[e]++)) appear[e] = idx;
  }
  void erase_edge(int idx, int u, int v){
    auto e = minmax(u,v);
    if(!(--cnt[e])) pend.emplace_back(appear[e], idx, e);
  }
  void _add(int l, int r, const edge& e){
    for(l += sz, r += sz; l < r; l >>= 1, r >>= 1) {
        if(l & 1) seg.add_edge(l++,e.first,e.second);
        if(r & 1) seg.add_edge(--r,e.first,e.second);
    }
  }
  void build(){
    for(auto &[e, c] : cnt) if(c) pend.emplace_back(appear[e],sz,e);
    for(auto &[l, r, e] : pend) _add(l,r,e);
    seg.build();
  }
  void run(const function<void(int)> &f){
    vector<int> states(szlog+1,0);
    int j, k = szlog;
    for(int i = 0; i < q; i++){
      for(j = i+sz; !(j&1); j >>= 1) k--;
      uf.rollback(states[k]);
      for(;j < sz; j <<= 1) {
        for(auto e : seg[j]) uf.merge(e.to,e.cost);
        states[++k] = uf.get_state();
      }
      for(auto e : seg[i+sz]) uf.merge(e.to,e.cost);
      f(i);
    }
  }

};

}  // namespace soryuusi
