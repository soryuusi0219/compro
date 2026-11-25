// https://github.com/soryuusi0219/compro/blob/main/library/graph/prim.hpp
#pragma once
#include <functional>
#include <queue>
#include <tuple>
#include <utility>
#include <vector>

namespace soryuusi {

using namespace std;

template<class T>
struct Prim{
  int n;
  vector<int> h, nxt, to;
  vector<T> w;
  Prim(int n_) : n(n_), h(n,-1) {}
  void add_edge(int u, int v, T cost){
    nxt.push_back(h[u]);
    h[u] = to.size();
    to.push_back(v);
    w.push_back(cost);
    nxt.push_back(h[v]);
    h[v] = to.size();
    to.push_back(u);
    w.push_back(cost);
  }
  pair<T,vector<pair<long long,long long>>> solve(int x = 0){
    if(!n) return {T(0),{}};
    priority_queue<tuple<T,int,int>,vector<tuple<T,int,int>>,greater<tuple<T,int,int>>> queue;
    vector<unsigned char> visited(n,0);
    vector<int> parent(n,-1);
    vector<T> best(n,T(0));
    vector<pair<long long,long long>> result;
    auto visit = [&](int u){
      visited[u] = true;
      for(int i = h[u]; i != -1; i = nxt[i]){
        const int v = to[i];
        if(visited[v]) continue;
        const T& weight = w[i];
        const T& current = best[v];
        if(parent[v] == -1 || tie(weight,u) < tie(current,parent[v])){
          best[v] = weight;
          parent[v] = u;
          queue.emplace(weight,u,v);
        }
      }
    };
    visit(x);
    T cost = 0;
    while(!queue.empty() && (int)result.size() < n-1){
      const auto [w,u,v] = queue.top();
      queue.pop();
      if(visited[v]) continue;
      cost += w;
      result.emplace_back(u,v);
      visit(v);
    }
    return {cost,move(result)};
  }
};

}  // namespace soryuusi
