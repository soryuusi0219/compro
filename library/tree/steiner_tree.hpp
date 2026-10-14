// https://github.com/soryuusi0219/compro/blob/main/library/tree/steiner_tree.hpp
#pragma once
#include <algorithm>
#include <functional>
#include <queue>
#include <tuple>
#include <utility>
#include <vector>

namespace soryuusi {

using namespace std;

template<class T, T e = 0, T infty = 1ll << 60, class INT>
pair<T,vector<tuple<INT,INT,T>>> steiner_tree(vector<vector<pair<INT, T>>>& G, vector<INT>& terminals){
  int n = G.size();
  int k = terminals.size();
  int K = 1 << k;
  if(k <= 1) return {e,{}};
  vector<vector<T>> dp(K, vector<T>(n, infty));
  vector<vector<pair<int,int>>> dp_back(K, vector<pair<int,int>>(n,{-1,-1}));
  dp[0] = vector<T>(n,e);
  for(int i = 0; i < k; i++) dp[1 << i][terminals[i]] = e;
  for(int i = 1; i < K; i++){
    priority_queue<pair<T, int>,vector<pair<T, int>>,greater<pair<T, int>>> q;
    for(int j = 0; j < n; j++) if(dp[i][j] < infty) q.push({dp[i][j],j});
    while(!q.empty()){
      auto p = q.top(); q.pop();
      int j = p.second;
      if(dp[i][j] < p.first) continue;
      for(auto [j2, w2] : G[j]) {
        if(dp[i][j] + w2 < dp[i][j2]) {
          dp[i][j2] = dp[i][j] + w2;
          dp_back[i][j2] = {K,j};
          q.push({dp[i][j2],j2});
       }
      }
    }
    for(int v = 0; v < n; v++){
      for(int j = (i+1)|i; j < min(2*i,K); j = (j+1)|i){
        if(dp[j][v] > dp[i][v] + dp[j-i][v]) {
          dp[j][v] = dp[i][v] + dp[j-i][v];
          dp_back[j][v] = {i,j-i};
        }
      }
    }
  }
  T resval = dp[K-1][terminals[0]];
  vector<tuple<INT,INT,T>> resedges;
  queue<pair<int,int>> q;
  q.push({K-1,terminals[0]});
  while(!q.empty()){
    auto [s,v] = q.front(); q.pop();
    if(!s || (__builtin_popcount(s) == 1 && terminals[__builtin_ctz(s)] == v)) continue;
    auto [i,j] = dp_back[s][v];
    if(i == K){
      if(i == -1) return {e,{}};
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

}  // namespace soryuusi
