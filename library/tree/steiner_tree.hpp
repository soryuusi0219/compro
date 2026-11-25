// https://github.com/soryuusi0219/compro/blob/main/library/tree/steiner_tree.hpp
#pragma once
#include <algorithm>
#include <functional>
#include <queue>
#include <tuple>
#include <utility>
#include <vector>

#include "../math/internal/internal_bit.hpp"
#include "../graph/static_graph.hpp"

namespace soryuusi {

using namespace std;

template<class T, T e = 0, T infty = 1ll << 60, class INT = int>
struct steiner_tree{
  int n;
  static_graph<T> G;
  steiner_tree(int n_) : n(n_), G(n) {}
  steiner_tree(const vector<vector<pair<INT,T>>>& G0) : n(G0.size()), G(G0) {}
  void add_edge(int u, int v, T cost, bool undirected = true){
    G.add_edge(u,v,cost,undirected);
  }
  pair<T,vector<tuple<INT,INT,T>>> solve(const vector<INT>& terminals){
    G.build();
    int k = terminals.size();
    int K = 1 << k;
    if(k <= 1) return {e,{}};
    vector<vector<T>> dp(K);
    vector<vector<pair<int,int>>> dp_back(K);
    vector<pair<T,int>> heap;
    heap.reserve(n);
    const greater<pair<T,int>> compare;
    for(int i = 1; i < K; i++){
      auto& distances = dp[i];
      auto& back = dp_back[i];
      distances.assign(n,infty);
      back.assign(n,{-1,-1});
      const unsigned mask = i;
      const int rest = mask & (mask-1);
      if(!rest) distances[terminals[soryuusi::countr_zero(mask)]] = e;
      for(int a = rest; a; a = (a-1)&rest){
        const int b = i^a;
        const auto& x = dp[a];
        const auto& y = dp[b];
        for(int v = 0; v < n; ++v){
          const T candidate = x[v]+y[v];
          if(candidate < distances[v]) distances[v] = candidate, back[v] = {a,b};
        }
      }
      for(int j = 0; j < n; ++j) if(distances[j] < infty) heap.emplace_back(distances[j],j);
      make_heap(heap.begin(),heap.end(),compare);
      while(!heap.empty()){
        pop_heap(heap.begin(),heap.end(),compare);
        const auto [cost,v] = heap.back();
        heap.pop_back();
        if(distances[v] < cost) continue;
        for(const auto& [to,w] : G[v]){
          const T candidate = cost+w;
          if(candidate < distances[to]){
            distances[to] = candidate;
            back[to] = {K,v};
            heap.emplace_back(candidate,to);
            push_heap(heap.begin(),heap.end(),compare);
          }
        }
      }
    }
    T resval = dp[K-1][terminals[0]];
    if(resval == infty) return {resval,{}};
    vector<tuple<INT,INT,T>> resedges;
    queue<pair<int,int>> q;
    q.push({K-1,terminals[0]});
    while(!q.empty()){
      auto [s,v] = q.front(); q.pop();
      if(!s || (soryuusi::popcount(static_cast<unsigned>(s)) == 1 && terminals[soryuusi::countr_zero(static_cast<unsigned>(s))] == v)) continue;
      auto [i,j] = dp_back[s][v];
      if(i == K){
        resedges.push_back({v,j, dp[s][v]-dp[s][j]});
        q.push({s,j});
      }
      else if(i < K){
        q.push({i,v});
        q.push({j,v});
      }
    }
    return {resval,resedges};
  }
};

}  // namespace soryuusi
