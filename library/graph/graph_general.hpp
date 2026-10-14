// https://github.com/soryuusi0219/compro/blob/main/library/graph/graph_general.hpp
#pragma once
#include <functional>
#include <tuple>
#include <utility>
#include <vector>

namespace soryuusi {

using namespace std;

vector<long long> get_path(const vector<vector<long long>>& G, int u, int v){
  vector<long long> ret;
  function<bool(int, int)> dfs = [&](int x, int p){
    if(x == u) {
      ret.push_back(x);
      return true;
    }
    for(int c : G[x])if(p != c){
      if(dfs(c,x)){
        ret.push_back(x);
        return true;
      }
    }
    return false;
  };
  dfs(v,-1);
  return ret;
}
template<class T>
vector<long long> get_path(const vector<vector<pair<long long, T>>>& G, int u, int v){
  vector<long long> ret;
  function<bool(int, int)> dfs = [&](int x, int p){
    if(x == u) {
      ret.push_back(x);
      return true;
    }
    for(auto [c,_] : G[x])if(p != c){
      if(dfs(c,x)){
        ret.push_back(x);
        return true;
      }
    }
    return false;
  };
  dfs(v,-1);
  return ret;
}
tuple<int, int, int> diameter(const vector<vector<long long>>& G){
  function<pair<int,int>(int, int, int)> dfs = [&](int v, int p, int d){
    pair<int,int> ret = {d,v};
    for(int c : G[v])if(p != c){
      auto cret = dfs(c,v,d+1);
      if(cret > ret) ret = cret;
    }
    return ret;
  };
  int u = dfs(0,-1,0).second;
  auto [d,v] = dfs(u,-1,0);
  if(u > v) swap(u,v);
  return {u,v,d};
}
template<class T>
tuple<int, int, T> diameter(const vector<vector<pair<long long, T>>>& G){
  function<pair<T,int>(int, int, T)> dfs = [&](int v, int p, T d){
    pair<T,int> ret = {d,v};
    for(auto [c,w] : G[v])if(p != c){
      auto cret = dfs(c,v,d+w);
      if(cret > ret) ret = cret;
    }
    return ret;
  };
  int u = dfs(0,-1,0).second;
  auto [d,v] = dfs(u,-1,0);
  if(u > v) swap(u,v);
  return {u,v,d};
}
pair<long long, long long> tree_get_centroids(const vector<vector<long long>>& G){
  int n = G.size();
  long long u = -1;
  long long v = -1;
  function<int(int, int)> dfs = [&](int x, int p){
    int sz = 1;
    for(int c : G[x])if(p != c){
      int csz = dfs(c,x);
      if(u >= 0) break;
      sz += csz;
    }
    if(2*sz >= n){
      u = x;
      v = (2*sz == n ? p : x);
    }
    return sz;
  };
  dfs(0,-1);
  if(u > v) swap(u,v);
  return {u,v};
}
template<class T>
pair<long long, long long> tree_get_centroids(const vector<vector<pair<long long, T>>>& G){
  int n = G.size();
  long long u = -1;
  long long v = -1;
  function<int(int, int)> dfs = [&](int x, int p){
    int sz = 1;
    for(auto [c,_] : G[x])if(p != c){
      int csz = dfs(c,x);
      if(u >= 0) break;
      sz += csz;
    }
    if(2*sz >= n){
      u = x;
      v = (2*sz == n ? p : x);
    }
    return sz;
  };
  dfs(0,-1);
  if(u > v) swap(u,v);
  return {u,v};
}

}  // namespace soryuusi
