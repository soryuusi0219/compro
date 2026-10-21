// https://github.com/soryuusi0219/compro/blob/main/library/graph/graph_general.hpp
#pragma once
#include <tuple>
#include <utility>
#include <vector>

namespace soryuusi {

using namespace std;

template<class INT>
vector<INT> get_path(const vector<vector<INT>>& G, int u, int v){
  vector<INT> ret;
  auto dfs = [&](auto&& self, int x, int p) -> bool {
    if(x == u) {
      ret.push_back(x);
      return true;
    }
    for(int c : G[x])if(p != c){
      if(self(self,c,x)){
        ret.push_back(x);
        return true;
      }
    }
    return false;
  };
  dfs(dfs,v,-1);
  return ret;
}
template<class T, class INT>
vector<INT> get_path(const vector<vector<pair<INT, T>>>& G, int u, int v){
  vector<INT> ret;
  auto dfs = [&](auto&& self, int x, int p) -> bool {
    if(x == u) {
      ret.push_back(x);
      return true;
    }
    for(auto [c,_] : G[x])if(p != c){
      if(self(self,c,x)){
        ret.push_back(x);
        return true;
      }
    }
    return false;
  };
  dfs(dfs,v,-1);
  return ret;
}
template<class INT>
tuple<int, int, int> diameter(const vector<vector<INT>>& G){
  auto dfs = [&](auto&& self, int v, int p, int d) -> pair<int,int> {
    pair<int,int> ret = {d,v};
    for(int c : G[v])if(p != c){
      auto cret = self(self,c,v,d+1);
      if(cret > ret) ret = cret;
    }
    return ret;
  };
  int u = dfs(dfs,0,-1,0).second;
  auto [d,v] = dfs(dfs,u,-1,0);
  if(u > v) swap(u,v);
  return {u,v,d};
}
template<class T, class INT>
tuple<int, int, T> diameter(const vector<vector<pair<INT, T>>>& G){
  auto dfs = [&](auto&& self, int v, int p, T d) -> pair<T,int> {
    pair<T,int> ret = {d,v};
    for(auto [c,w] : G[v])if(p != c){
      auto cret = self(self,c,v,d+w);
      if(cret > ret) ret = cret;
    }
    return ret;
  };
  int u = dfs(dfs,0,-1,0).second;
  auto [d,v] = dfs(dfs,u,-1,0);
  if(u > v) swap(u,v);
  return {u,v,d};
}
template<class INT>
pair<int, int> tree_get_centroids(const vector<vector<INT>>& G){
  int n = G.size();
  int u = -1;
  int v = -1;
  auto dfs = [&](auto&& self, int x, int p) -> int {
    int sz = 1;
    for(int c : G[x])if(p != c){
      int csz = self(self,c,x);
      if(u >= 0) break;
      sz += csz;
    }
    if(2*sz >= n){
      u = x;
      v = (2*sz == n ? p : x);
    }
    return sz;
  };
  dfs(dfs,0,-1);
  if(u > v) swap(u,v);
  return {u,v};
}
template<class T, class INT>
pair<int, int> tree_get_centroids(const vector<vector<pair<INT, T>>>& G){
  int n = G.size();
  int u = -1;
  int v = -1;
  auto dfs = [&](auto&& self, int x, int p) -> int {
    int sz = 1;
    for(auto [c,_] : G[x])if(p != c){
      int csz = self(self,c,x);
      if(u >= 0) break;
      sz += csz;
    }
    if(2*sz >= n){
      u = x;
      v = (2*sz == n ? p : x);
    }
    return sz;
  };
  dfs(dfs,0,-1);
  if(u > v) swap(u,v);
  return {u,v};
}

}  // namespace soryuusi
