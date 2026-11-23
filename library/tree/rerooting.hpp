// https://github.com/soryuusi0219/compro/blob/main/library/tree/rerooting.hpp
#pragma once
#include <type_traits>
#include <utility>
#include <vector>

namespace soryuusi {

using namespace std;

template<class S, auto op, auto e, class F, auto mapping>
struct Rerooting{
  static_assert(is_invocable_r_v<S, decltype(op), S, S>, "op must be callable with (S, S) and return a value convertible to S");
  static_assert(is_invocable_r_v<S, decltype(e)>, "e must be callable with no arguments and return a value convertible to S");
  static_assert(is_invocable_r_v<S, decltype(mapping), F, S>, "mapping must be callable with (F, S) and return a value convertible to S");

  int n;
  vector<S> data;
  vector<int> h, nxt, to;
  vector<F> fs;
  int m = 0;
  vector<S> edge_answers;
  vector<S> vertex_answers;
  Rerooting(int n0) : n(n0), h(n,-1){
    data.resize(n);
    reserve_edges();
  }
  Rerooting(int n0, vector<S> data0) : n(n0), data(std::move(data0)), h(n,-1){reserve_edges();}
  void set(int i, S x){data[i] = x;}
  int add_edge(int i, int j, F f){
    nxt.push_back(h[i]);
    h[i] = to.size();
    to.push_back(j);
    fs.push_back(f);
    nxt.push_back(h[j]);
    h[j] = to.size();
    to.push_back(i);
    fs.push_back(f);
    return m++;
  }
  int add_edge(int i, int j, F f, F g){
    nxt.push_back(h[i]);
    h[i] = to.size();
    to.push_back(j);
    fs.push_back(g);
    nxt.push_back(h[j]);
    h[j] = to.size();
    to.push_back(i);
    fs.push_back(f);
    return m++;
  }
  void build(){
    edge_answers.resize(m << 1);
    vertex_answers.resize(n);
    if(!n) return;
    vector<pair<int,int>> order;
    order.reserve(n);
    order.emplace_back(0,-1);
    for(int i = 0; i < n; ++i){
      const auto [v,parent] = order[i];
      for(int edge = h[v]; edge != -1; edge = nxt[edge]){
        if(edge != parent) order.emplace_back(to[edge],edge^1);
      }
    }
    for(int i = n-1; i >= 0; --i){
      const auto [v,parent] = order[i];
      S children = e();
      for(int edge = h[v]; edge != -1; edge = nxt[edge])
        if(edge != parent) children = op(edge_answers[edge],children);
      vertex_answers[v] = children;
      if(parent != -1) edge_answers[parent^1] = mapping(fs[parent^1],op(data[v],children));
    }
    vector<pair<int,S>> prefix;
    for(const auto& [v,parent] : order){
      const S x = parent == -1 ? data[v] : op(edge_answers[parent],data[v]);
      vertex_answers[v] = op(vertex_answers[v],x);
      S left = e();
      for(int edge = h[v]; edge != -1; edge = nxt[edge]) if(edge != parent){
        prefix.emplace_back(edge,left);
        left = op(edge_answers[edge],left);
      }
      S right = e();
      for(auto it = prefix.rbegin(); it != prefix.rend(); ++it){
        const auto& [edge,other] = *it;
        edge_answers[edge^1] = mapping(fs[edge^1],op(op(x,other),right));
        right = op(right,edge_answers[edge]);
      }
      prefix.clear();
    }
  }
  vector<S> answers(){return vertex_answers;}
  S answer(int v){return vertex_answers[v];}
  vector<S> subanswers(int v){
    vector<S> ret;
    for(int id = h[v]; ~id; id = nxt[id]) ret.push_back(edge_answers[id]);
    return ret;
  }
  S subanswer(int id){return edge_answers[id];}

private:
  void reserve_edges(){
    const int edges = n ? 2*(n-1) : 0;
    nxt.reserve(edges);
    to.reserve(edges);
    fs.reserve(edges);
  }
};

}  // namespace soryuusi
