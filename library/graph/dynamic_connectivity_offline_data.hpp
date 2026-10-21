// https://github.com/soryuusi0219/compro/blob/main/library/graph/dynamic_connectivity_offline_data.hpp
#pragma once
#include <algorithm>
#include <cassert>
#include <map>
#include <stack>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

#include "library/data_structure/dsu_undo_with_data.hpp"
#include "library/graph/static_graph.hpp"

namespace soryuusi {

using namespace std;

template<class T, auto op>
struct dynamic_connectivity_offline_data{
  static_assert(is_invocable_r_v<T, decltype(op), T, T>, "op must be callable with (T, T) and return a value convertible to T");

  dsu_undo_data<T,op> uf;
  using edge = pair<int,int>;
  int n, q;
  int sz, szlog;
  static_graph<int> sege;
  static_graph<T> segv;
  map<edge, int> cnt, appear;
  vector<tuple<int,int,edge>> pende;
  vector<tuple<int,int,T>> pendv;

  dynamic_connectivity_offline_data(int n_, T v, int q_) : uf(n_, v), n(n_), q(q_){
    sz = 1, szlog = 0; while(sz < q) sz <<= 1, szlog++;
    sege = static_graph<int>(sz << 1);
    segv = static_graph<T>(sz << 1);
  }
  dynamic_connectivity_offline_data(const vector<T> & v, int q_) : uf(v), n(v.size()), q(q_){
    sz = 1, szlog = 0; while(sz < q) sz <<= 1, szlog++;
    sege = static_graph<int>(sz << 1);
    segv = static_graph<T>(sz << 1);
  }

  void add_edge(int idx, int u, int v){
    auto e = minmax(u,v);
    if(!(cnt[e]++)) appear[e] = idx;
  }
  void erase_edge(int idx, int u, int v){
    auto e = minmax(u,v);
    if(!(--cnt[e])) pende.emplace_back(appear[e], idx, e);
  }
  void apply(int idx, int x, T v){
    pendv.emplace_back(idx, x, v);
  }
  void _adde(int l, int r, const edge& e){
    for(l += sz, r += sz; l < r; l >>= 1, r >>= 1) {
      if(l & 1) sege.add_edge(l++,e.first,e.second);
      if(r & 1) sege.add_edge(--r,e.first,e.second);
    }
  }
  void _addv(int l, int x, T v){
    int r = sz << 1;
    for(l += sz; l < r; l >>= 1, r >>= 1) {
      if(l & 1) segv.add_edge(l++,x,v);
    }
  }
  void build(){
    for(auto &[e, c] : cnt) if(c) pende.emplace_back(appear[e],sz,e);
    for(auto &[l, r, e] : pende) _adde(l,r,e);
    for(auto &[l, x, v] : pendv) _addv(l,x,v);
    sege.build();
    segv.build();
  }
  template<class F>
  void run(F&& f){
    static_assert(is_invocable_r_v<void, F, int>);
    vector<int> states(szlog+1,0);
    int j, k = szlog;
    for(int i = 0; i < q; i++){
      for(j = i+sz; !(j&1); j >>= 1) k--;
      uf.rollback(states[k]);
      for(;j < sz; j <<= 1) {
        for(auto e : sege[j]) uf.merge(e.to,e.cost);
        for(auto e : segv[j]) uf.apply(e.to,e.cost);
        states[++k] = uf.get_state();
      }
      for(auto e : sege[i+sz]) uf.merge(e.to,e.cost);
      for(auto e : segv[i+sz]) uf.apply(e.to,e.cost);
      f(i);
    }
  }

};

}  // namespace soryuusi
