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
    priority_queue<tuple<T,int,int>,vector<tuple<T,int,int>>,greater<tuple<T,int,int>>> que;
    vector<bool> v(n,false);
    v[0] = true;
    for(int i = h[x]; ~i; i = nxt[i]) que.push({w[i],x,to[i]});
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
      for(int i = h[u2]; ~i; i = nxt[i]) if(!v[to[i]]) que.push({w[i],u2,to[i]});
    }
    return {cost,ret};
  }
};

}  // namespace soryuusi
