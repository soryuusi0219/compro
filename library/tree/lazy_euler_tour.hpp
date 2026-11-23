// https://github.com/soryuusi0219/compro/blob/main/library/tree/lazy_euler_tour.hpp
#pragma once
#include <algorithm>
#include <type_traits>
#include <utility>
#include <vector>

#include "../data_structure/lazy_segtree.hpp"
#include "../graph/static_graph.hpp"
#include "simple_euler_tour.hpp"

namespace soryuusi {

using namespace std;

template<class S, auto op, auto e, class F, auto mapping, auto composition, auto id>
struct lazy_EularTour{
  static_assert(is_invocable_r_v<S, decltype(op), S, S>, "op must be callable with (S, S) and return a value convertible to S");
  static_assert(is_invocable_r_v<S, decltype(e)>, "e must be callable with no arguments and return a value convertible to S");
  static_assert(is_invocable_r_v<S, decltype(mapping), F, S>, "mapping must be callable with (F, S) and return a value convertible to S");
  static_assert(is_invocable_r_v<F, decltype(composition), F, F>, "composition must be callable with (F, F) and return a value convertible to F");
  static_assert(is_invocable_r_v<F, decltype(id)>, "id must be callable with no arguments and return a value convertible to F");

  static constexpr int inf = 1ll << 30;
  using StaticRMQ = SimpleEularTour::StaticRMQ;
  int N = 0;
  bool is_built = false;
  static_graph<> G;
  vector<int> invertex, outvertex, tourid;
  vector<int> inseg, outseg;
  vector<S> data;
  StaticRMQ depth;
  LazySegtree<S,op,e,F,mapping,composition,id> seg;
  lazy_EularTour(int n) : N(n), G(N,2*N-2) {
    invertex.resize(N);
    outvertex.resize(N);
    tourid.resize(2*N-1);
    data.resize(N);
    inseg.resize(N);
    outseg.resize(N);
  }
  lazy_EularTour(const vector<S>& a) : N(a.size()), G(N,2*N-2), data(a){
    invertex.resize(N);
    outvertex.resize(N);
    tourid.resize(2*N-1);
    inseg.resize(N);
    outseg.resize(N);
  }
  void add_edge(int u, int v){
    G.add_edge(u,v,true);
  }
  void build(int root = 0){
    int j = 0;
    vector<S> datavec;
    datavec.reserve(N);
    depth.build(internal::build_euler_tour(G,root,invertex,outvertex,tourid, [&](int v){inseg[v] = j++; datavec.push_back(data[v]);}, [&](int v){outseg[v] = j;}));
    seg = LazySegtree<S,op,e,F,mapping,composition,id>(datavec);
    is_built = true;
  }
  int get_depth(int u){
    return depth.data[invertex[u]].first;
  }
  int lca(int u, int v){
    if(u == v) return u;
    if(invertex[u] > invertex[v]) swap(u,v);
    return depth.prod(invertex[u],invertex[v]+1).second;
  }
  void set(int v, S x){
    if(is_built) seg.set(inseg[v],x);
    else data[v] = x;
  }
  void apply(int v, F f, bool contains_root = true){
    seg.apply(inseg[v] + !contains_root, outseg[v], f);
  }
  S get(int v){
    return seg.get(inseg[v]);
  }
  S prod(int v, bool contains_root = true){
    return seg.prod(inseg[v] + !contains_root, outseg[v]);
  }
};

}  // namespace soryuusi
