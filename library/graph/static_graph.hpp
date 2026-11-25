// https://github.com/soryuusi0219/compro/blob/main/library/graph/static_graph.hpp
#pragma once
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <numeric>
#include <type_traits>
#include <utility>
#include <vector>

namespace soryuusi {

using namespace std;

template<class T>
struct static_graph_edge{
  int to;
  T cost;
};

template<>
struct static_graph_edge<void>{
  int to;
};

template<class T = void>
struct static_graph{
  using edge = static_graph_edge<T>;
  using edge_iterator = typename vector<edge>::const_iterator;
  struct edge_range{
    edge_iterator l, r;
    edge_iterator begin() const{return l;}
    edge_iterator end() const{return r;}
  };
  struct vertex_iterator{
    edge_iterator i;
    int operator*() const{return i->to;}
    vertex_iterator& operator++(){++i; return *this;}
    bool operator!=(const vertex_iterator& rhs) const{return i != rhs.i;}
  };
  struct vertex_range{
    vertex_iterator l, r;
    vertex_iterator begin() const{return l;}
    vertex_iterator end() const{return r;}
  };

  int n;
  bool initialized = false;
  vector<int> start, from;
  vector<edge> es;
  static_graph(int n_ = 0) : n(n_) {}
  static_graph(int n_, int m) : n(n_) {from.reserve(m), es.reserve(m);}
  template<class I> requires (is_void_v<T> && is_convertible_v<I,int>)
  static_graph(const vector<vector<I>>& G) : static_graph(G.size()){
    build_from_adjacency(G,[](int v){return edge{v};});
  }
  template<class I, class U> requires (!is_void_v<T> && is_convertible_v<I,int> && is_convertible_v<U,T>)
  static_graph(const vector<vector<pair<I,U>>>& G) : static_graph(G.size()){
    build_from_adjacency(G,[](const auto& e){return edge{static_cast<int>(e.first),static_cast<T>(e.second)};});
  }
  void add_edge(int u, int v, bool undirected = false) requires is_void_v<T>{
    assert(!initialized);
    from.push_back(u), es.push_back({v});
    if(undirected) from.push_back(v), es.push_back({u});
  }
  template<class U = T> requires (!is_void_v<T> && is_convertible_v<U,T>)
  void add_edge(int u, int v, U w, bool undirected = false){
    assert(!initialized);
    from.push_back(u), es.push_back({v,w});
    if(undirected) from.push_back(v), es.push_back({u,w});
  }
  void build(){
    if(initialized) return;
    start.assign(n+1,0);
    if(is_sorted(from.begin(),from.end())){
      for(int v : from) ++start[v+1];
      partial_sum(start.begin(),start.end(),start.begin());
    }else{
      for(int v : from) ++start[v];
      partial_sum(start.begin(),start.end(),start.begin());
      auto sorted = [&]{
        if constexpr (is_default_constructible_v<edge>) return vector<edge>(es.size());
        else return es;
      }();
      // In reverse insertion order, the end offsets become start offsets.
      for(int i = (int)es.size()-1; i >= 0; --i) sorted[--start[from[i]]] = move(es[i]);
      es = move(sorted);
    }
    vector<int>().swap(from);
    initialized = true;
  }
  edge_range operator[](int v) const{return {es.begin()+start[v],es.begin()+start[v+1]};}
  edge_range edges(int v) const{return (*this)[v];}
  vertex_range vertices(int v) const{return {{es.begin()+start[v]},{es.begin()+start[v+1]}};}
  int degree(int v) const{return start[v+1]-start[v];}
  int size() const{return n;}
  int edge_size() const{return es.size();}

private:
  template<class Rows, class Convert>
  void build_from_adjacency(const Rows& G, Convert convert){
    size_t m = 0;
    for(const auto& row : G) m += row.size();
    es.reserve(m);
    start.resize(n+1);
    for(int i = 0; i < n; ++i){
      for(const auto& e : G[i]) es.push_back(convert(e));
      start[i+1] = es.size();
    }
    initialized = true;
  }
};

#define readg_static(G,n,m) static_graph<> G(n,m);for(int _=0;_<m;++_){int a,b;cin>>a>>b;G.add_edge(--a,--b,true);}G.build()
#define readgd_static(G,n,m) static_graph<> G(n,m);for(int _=0;_<m;++_){int a,b;cin>>a>>b;G.add_edge(--a,--b,false);}G.build()
#define readgdrev_static(G,n,m) static_graph<> G(n,m);for(int _=0;_<m;++_){int a,b;cin>>a>>b;G.add_edge(--b,--a,false);}G.build()
#define readt_static(G,n) readg_static(G,n,n-1)
#define readtp_static(G,n) static_graph<> G(n,n-1);for(int a=1;a<n;++a){int b;cin>>b;G.add_edge(a,--b,true);}G.build()

}  // namespace soryuusi
