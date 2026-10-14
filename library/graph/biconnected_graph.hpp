// https://github.com/soryuusi0219/compro/blob/main/library/graph/biconnected_graph.hpp
#pragma once
#include <algorithm>
#include <functional>
#include <queue>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

namespace soryuusi {

using namespace std;


struct biconnected_graph{
  vector<vector<int>> G;
  int n;
  vector<int> ord;
  vector<int> low;
  bool initialized = false;
  vector<vector<int>> tree;
  biconnected_graph(vector<vector<int>>& G) : G(G), n(G.size()), ord(vector<int>(n)), low(vector<int>(n)){}
  biconnected_graph(int n) : G(vector<vector<int>>(n)), n(n), ord(vector<int>(n)), low(vector<int>(n)){}
  void add_edge(int a, int b){G[a].push_back(b); G[b].push_back(a); initialized = false;}
  void low_link(){
    if(initialized) return;
    initialized = true;
    vector<bool> visited(n,false);
    tree = vector<vector<int>>(n);
    function<void(int, int, int)> dfs = [&](int v, int p, int noword = 0){
      visited[v] = true;
      ord[v] = noword;
      low[v] = noword;
      for(auto c:G[v]) if(c != p){
        if(!visited[c]){
          tree[v].push_back(c);
          tree[c].push_back(v);
          dfs(c,v,noword+1);
          low[v] = min(low[v],low[c]);
        }else{
          if(ord[c] < ord[v]) low[v] = min(low[v],ord[c]);
          else low[c] = min(low[c],ord[v]);
        }
      }
    };
    for(int i = 0; i < n; i++) if(!visited[i]) dfs(i,-1,0);
  }
  bool is_bridge(int u, int v){
    if(!initialized) low_link();
    return low[v] > ord[u] || low[u] > ord[v];
  }
  vector<long long> two_edge_connected_component(int v){
    if(!initialized) low_link();
    vector<long long> ret;
    function<void(int, int)> dfs = [&](int v, int p){
      ret.push_back(v);
      for(auto c: tree[v]) if(c != p && !is_bridge(v,c)) dfs(c,v);
    };
    dfs(v,-1);
    return ret; 
  }
  tuple<vector<vector<long long>>,vector<long long>,vector<vector<long long>>> two_edge_connected_components(){
    if(!initialized) low_link();
    vector<vector<long long>> retG;
    vector<long long> ind(n,-1);
    vector<vector<long long>> grps;
    queue<int> q;
    int k = 0;
    int m = 0;
    function<void(int, int)> dfs = [&](int v, int p){
      ind[v] = k;
      grps[k].push_back(v);
      for(auto c: tree[v]) if(c != p){
        if(is_bridge(v,c)){
          if(ind[c] == -1){
            retG.push_back({k});
            retG[k].push_back(m);
            m++;
            q.push(c); 
          }
        }else dfs(c,v);
      }
    };
    for(int i = 0; i < n; i++){
      if(ind[i] != -1) continue;
      q.push(i);
      retG.push_back({});
      m++;
      while(!q.empty()){
        grps.push_back({});
        dfs(q.front(),-1);
        q.pop();
        k++;
      }
    } 
    return {retG,ind,grps};
  }
  vector<vector<long long>> two_vertex_connected_components(){
    if(!initialized) low_link();
    vector<vector<long long>> grps;
    queue<pair<int,int>> q;
    function<void(int, int)> dfs = [&](int v, int p){
      grps.back().push_back(v);
      for(auto c: tree[v]) if(c != p){
        if(low[c] >= ord[v]) q.push({c,v});
        else dfs(c,v);
      }
    };
    for(int i = 0; i < n; i++){
      if(ord[i]) continue;
      if(tree[i].empty()) {
        grps.push_back({i});
        continue;
      }
      for(int j : tree[i]) q.push({j,i});
      while(!q.empty()){
        grps.push_back({q.front().second});
        dfs(q.front().first, q.front().second);
        q.pop();
      }
    } 
    return grps;
  }
  pair<vector<vector<long long>>,vector<long long>> block_cut_tree(){
    if(!initialized) low_link();
    vector<vector<long long>> rett;
    vector<long long> ind(n,-1);
    queue<tuple<int,int,int>> q;
    int m = 0;
    int k;
    function<void(int, int)> dfs = [&](int v, int p){
      for(auto c: tree[v]) if(c != p){
        if(low[c] >= ord[v]) {
          if(ind[v] == -1){
            rett.push_back({k});
            rett[k].push_back(m);
            ind[v] = m;
            m++;
          }
          q.push({c,v,ind[v]});
        }
        else dfs(c,v);
      }
      if(ind[v] == -1) ind[v] = k;
    };
    for(int i = 0; i < n; i++){
      if(ord[i]) continue;
      if(tree[i].empty()) {
        rett.push_back({});
        ind[i] = m;
        m++;
        continue;
      }
      if(tree[i].size() == 1) q.push({tree[i][0],i,-1});
      else{
        rett.push_back({});
        ind[i] = m;
        for(int j : tree[i]) q.push({j,i,m});
        m++;
      }
      while(!q.empty()){
        auto [u,v,x] = q.front();
        rett.push_back({});
        if(x != -1){
          rett[x].push_back(m);
          rett[m].push_back(x);
        }else ind[v] = m;
        k = m++;
        dfs(u, v);
        q.pop();
      }
    } 
    return {rett,ind};
  }
};

}  // namespace soryuusi
