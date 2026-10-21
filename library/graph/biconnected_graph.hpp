// https://github.com/soryuusi0219/compro/blob/main/library/graph/biconnected_graph.hpp
#pragma once
#include <algorithm>
#include <functional>
#include <queue>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

#include <iostream>

#include "library/graph/static_graph.hpp"

namespace soryuusi {

using namespace std;


struct biconnected_graph{
  int n;
  static_graph<> G;
  vector<int> h, nxt, ord, low, par;
  bool initialized = false;
  biconnected_graph() {}
  biconnected_graph(int n_) : n(n_), G(n), ord(n), low(n), par(n,-1) {}
  biconnected_graph(int n_, int m) : n(n_), G(n,2*m), ord(n), low(n), par(n,-1) {}
  void add_edge(int u, int v){
    G.add_edge(u,v,true);
  }
  void low_link(){
    if(initialized) return;
    initialized = true;
    G.build();
    vector<bool> seen(n,false);
    auto dfs = [&](auto&& self, int v, int p, int noword = 0) -> void {
      par[v] = p;
      seen[v] = true;
      ord[v] = noword, low[v] = noword;
      for(int c : G.vertices(v)) {
        if(c == p) continue;
        if(!seen[c]){
          self(self,c,v,noword+1);
          low[v] = min(low[v],low[c]);
        }else{
          if(ord[c] < ord[v]) low[v] = min(low[v],ord[c]);
          else low[c] = min(low[c],ord[v]);
        }
      }
    };
    for(int i = 0; i < n; ++i) if(!seen[i]) dfs(dfs,i,-1,0);
    h.assign(par.begin(),par.end());
    nxt.assign(n,-1);
    for(int c = 0; c < n; ++c) if(~par[c]) nxt[c] = h[par[c]], h[par[c]] = c;
  }
  bool is_bridge(int u, int v){
    if(!initialized) low_link();
    return low[v] > ord[u] || low[u] > ord[v];
  }
  vector<int> two_edge_connected_component(int v){
    if(!initialized) low_link();
    vector<int> res;
    auto dfs = [&](auto&& self, int v) -> void {
      res.push_back(v);
      for(int c = h[v]; c != par[v]; c = nxt[c]) if(!is_bridge(v,c)) self(self,c);
    };
    while(~par[v] && !is_bridge(par[v],v)) v = par[v];
    dfs(dfs,v);
    return res;
  }
  vector<vector<int>> two_edge_connected_components(){
    if(!initialized) low_link();
    vector<vector<int>> grps;
    vector<int> q;
    q.reserve(n);
    int m = 0;
    auto dfs = [&](auto self, int v) -> void {
      grps.back().push_back(v);
      for(int c = h[v]; c != par[v]; c = nxt[c]){
        if(is_bridge(v,c)){
          m++;
          q.push_back(c);
        }else self(self,c);
      }
    };
    for(int i = 0; i < n; i++){
      if(~par[i]) continue;
      q.push_back(i);
      m++;
      while(q.size()){
        int v = q.back(); q.pop_back();
        grps.emplace_back();
        dfs(dfs,v);
      }
    } 
    return grps;
  }
  // (compressed tree, index, grpups)
  tuple<vector<vector<int>>,vector<int>,vector<vector<int>>> two_edge_connected_compressed(){
    if(!initialized) low_link();
    vector<vector<int>> rest;
    vector<int> ind(n,-1);
    vector<vector<int>> grps;
    queue<int> q;
    int k = 0;
    int m = 0;
    auto dfs = [&](auto self, int v) -> void {
      ind[v] = k;
      grps[k].push_back(v);
      for(int c = h[v]; c != par[v]; c = nxt[c]){
        if(is_bridge(v,c)){
          if(ind[c] == -1){
            rest.emplace_back(1,k);
            rest[k].push_back(m);
            m++;
            q.push(c); 
          }
        }else self(self,c);
      }
    };
    for(int i = 0; i < n; i++){
      if(ind[i] != -1) continue;
      q.push(i);
      rest.emplace_back();
      m++;
      while(q.size()){
        int v = q.front(); q.pop();
        grps.emplace_back();
        dfs(dfs,v);
        k++;
      }
    } 
    return {rest,ind,grps};
  }
  vector<vector<int>> two_vertex_connected_components(){
    if(!initialized) low_link();
    vector<vector<int>> grps;
    vector<int> q;
    auto dfs = [&](auto&& self, int v) -> void {
      grps.back().push_back(v);
      for(int c = h[v]; c != par[v]; c = nxt[c]){
        if(low[c] >= ord[v]) q.emplace_back(c);
        else self(self,c);
      }
    };
    for(int i = 0; i < n; i++){
      if(ord[i]) continue;
      if(!~h[i]) {
        grps.emplace_back(1,i);
        continue;
      }
      for(int c = h[i]; ~c; c = nxt[c]) q.emplace_back(c);
      while(!q.empty()){
        auto v = q.back(); q.pop_back();
        grps.emplace_back(1,par[v]);
        dfs(dfs,v);
      }
    } 
    return grps;
  }
  pair<vector<vector<int>>,vector<int>> block_cut_tree(){
    if(!initialized) low_link();
    vector<vector<int>> rest;
    vector<int> ind(n,-1);
    vector<pair<int,int>> q;
    int m = 0;
    int k;
    auto dfs = [&](auto&& self, int v) -> void {
      for(int c = h[v]; c != par[v]; c = nxt[c]){
        if(low[c] >= ord[v]) {
          if(ind[v] == -1){
            rest.emplace_back(1,k);
            rest[k].push_back(m);
            ind[v] = m;
            m++;
          }
          q.emplace_back(c,ind[v]);
        }
        else self(self,c);
      }
      if(ind[v] == -1) ind[v] = k;
    };
    for(int i = 0; i < n; i++){
      if(ord[i]) continue;
      if(!~h[i]) {
        rest.emplace_back();
        ind[i] = m;
        m++;
        continue;
      }
      if(!~nxt[h[i]]) q.emplace_back(h[i],-1);
      else{
        rest.emplace_back();
        ind[i] = m;
        for(int c = h[i]; ~c; c = nxt[c]) q.emplace_back(c,m);
        m++;
      }
      while(!q.empty()){
        auto [u,x] = q.back();
        q.pop_back();
        rest.emplace_back();
        if(x != -1){
          rest[x].push_back(m);
          rest[m].push_back(x);
        }else ind[par[u]] = m;
        k = m++;
        dfs(dfs,u);
      }
    } 
    return {rest,ind};
  }
};

}  // namespace soryuusi
