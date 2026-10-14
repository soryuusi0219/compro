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
  struct Edge{
    int to;
    T cost;
  };
  using Graph = vector<vector<Edge>>;
  Graph G;
  Prim(int n){
    G.resize(n);
  }
  void add_edge(int u, int v, T cost){
    G[u].push_back(Edge{v,cost});
    G[v].push_back(Edge{u,cost});
  }
  pair<T,vector<pair<long long,long long>>> solve(int x = 0){
    int n = G.size();
    priority_queue<tuple<T,int,int>,vector<tuple<T,int,int>>,greater<tuple<T,int,int>>> que;
    vector<bool> v(n,false);
    v[0] = true;
    for(auto e : G[x]) que.push({e.cost,x,e.to});
    vector<pair<long long,long long>> ret;
    T cost = 0;
    int v_size = 0;
    while(v_size < n-1){
      if(que.empty()) break;
      tuple<T,int,int> t = que.top(); que.pop();
      T c = get<0>(t); int u1 = get<1>(t), u2 = get<2>(t);
      if(v[u2]) continue;
      v_size++;
      v[u2] = true;
      cost += c;
      ret.push_back({u1,u2});
      for(auto e : G[u2]) if(!v[e.to]) que.push({e.cost,u2,e.to});
    }
    return {cost,ret};
  }
};

}  // namespace soryuusi
