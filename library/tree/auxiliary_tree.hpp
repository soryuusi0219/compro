// https://github.com/soryuusi0219/compro/blob/main/library/tree/auxiliary_tree.hpp
#pragma once
#include <algorithm>
#include <cassert>
#include <stack>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <vector>

namespace soryuusi {

using namespace std;

struct Auxiliary_Tree{
  int n;
  vector<int> h, nxt, to;
  vector<vector<int>> ancs;
  vector<int> depth;
  vector<int> ord;
  int root;
  Auxiliary_Tree(int n0, int r = 0) : n(n0), h(n,-1){
    ancs.resize(n);
    depth.resize(n);
    ord.resize(n);
    root = r;
  }
  void add_edge(int u,int v){
    assert(0 <= u && u < n);
    assert(0 <= v && v < n);
    nxt.push_back(h[u]);
    h[u] = to.size();
    to.push_back(v);
    nxt.push_back(h[v]);
    h[v] = to.size();
    to.push_back(u);
  }
  void set_up(){
    auto dfs = [&](auto&& self, int v, int p, int dep, int& cnt) -> void {
      ord[v] = cnt;
      cnt++;
      ancs[v].push_back(p);
      while(ancs[v].back() != root) ancs[v].push_back(ancs[ancs[v].back()][min(ancs[v].size(), ancs[ancs[v].back()].size())-1]);
      depth[v] = dep;
      for(int i = h[v]; ~i; i = nxt[i]) if(to[i] != p){
        int c = to[i];
        self(self,c,v,dep+1,cnt);
      }
    };
    int cnt = 0;
    dfs(dfs,root,root,0,cnt);
  }
  int up(int v, int k){
    if(n < k) k = n;
    int i = 0;
    while(k){
      if(k & 1) {
        if((int)ancs[v].size() <= i) return root;
        v = ancs[v][i];
      }
      i++;
      k = k >> 1;
    }
    return v;
  }
  int lca(int u, int v){
    assert(0 <= u && u < n);
    assert(0 <= v && v < n);
    if(depth[v] < depth[u]) swap(u,v);
    v = up(v,depth[v] - depth[u]);
    if(u == v) return u;
    int i = ancs[u].size()-1;
    while(i >= 0){
      if(i < (int)ancs[u].size() && i < (int)ancs[v].size() && ancs[u][i] != ancs[v][i]){
        u = ancs[u][i];
        v = ancs[v][i];
      } 
      i--;
    }
    return ancs[u][0];
  }
  tuple<vector<vector<long long>>,vector<long long>, long long> auxiliary_tree(vector<long long> s){
    sort(s.begin(), s.end(), [&](long long lhs, long long rhs){return ord[lhs] < ord[rhs];});
    stack<int> st;
    unordered_map<int,int> m;
    vector<vector<long long>> retg;
    int cnt = 0;
    st.push(s[0]);
    m[st.top()] = cnt;
    cnt++;
    vector<bool> used;
    for(int i = 1; i < (int)s.size(); i++){
      int x = s[i];
      int w = lca(st.top(),x);
      if(!m.count(w)){
        m[w] = cnt;
        cnt++;
      }
      if(!m.count(x)){
        m[x] = cnt;
        cnt++;
      }
      while(!st.empty() && depth[st.top()] >= depth[w]){
        int v = st.top();
        st.pop();
        retg.resize(cnt);
        used.resize(cnt);
        if(!st.empty() && depth[st.top()] >= depth[w]){
          if(!used[m[v]] && v != st.top()) {
            retg[m[st.top()]].push_back(m[v]);
            used[m[v]] = 1;
          }
        }else{
          if(!used[m[v]] && v != w) {
            retg[m[w]].push_back(m[v]);
            used[m[v]] = 1;
          }
        }
      }
      st.push(w);
      st.push(x);
    }
    long long retr;
    while(!st.empty()){
      if(!m.count(st.top())){
        m[st.top()] = cnt;
        cnt++;
      }
      int v = st.top();
      st.pop();
      retg.resize(cnt);
      used.resize(cnt);
      if(!st.empty()){
        if(!used[m[v]] && v != st.top()) {
          retg[m[st.top()]].push_back(m[v]);
          used[m[v]] = 1;
        }
      }
      else retr = m[v];
    }
    vector<long long> ind(cnt);
    for(auto[i,j]:m) ind[j] = i;
    return {retg,ind,retr};
  }

  tuple<vector<vector<pair<long long,long long>>>,vector<long long>,long long> auxiliary_tree_dist(vector<long long> s){
    sort(s.begin(), s.end(), [&](long long lhs, long long rhs){return ord[lhs] < ord[rhs];});
    stack<int> st;
    unordered_map<int,int> m;
    vector<vector<pair<long long,long long>>> retg;
    int cnt = 0;
    st.push(s[0]);
    m[st.top()] = cnt;
    cnt++;
    vector<bool> used;
    for(int i = 1; i < (int)s.size(); i++){
      int x = s[i];
      int w = lca(st.top(),x);
      if(!m.count(w)){
        m[w] = cnt;
        cnt++;
      }
      if(!m.count(x)){
        m[x] = cnt;
        cnt++;
      }
      while(!st.empty() && depth[st.top()] >= depth[w]){
        int v = st.top();
        st.pop();
        retg.resize(cnt);
        used.resize(cnt);
        if(!st.empty() && depth[st.top()] >= depth[w]){
          if(!used[m[v]] && v != st.top()) {
            retg[m[st.top()]].push_back({m[v],depth[v] - depth[st.top()]});
            used[m[v]] = 1;
          }
        }else{
          if(!used[m[v]] && v != w) {
            retg[m[w]].push_back({m[v],depth[v] - depth[w]});
            used[m[v]] = 1;
          }
        }
      }
      st.push(w);
      st.push(x);
    }
    long long retr;
    while(!st.empty()){
      if(!m.count(st.top())){
        m[st.top()] = cnt;
        cnt++;
      }
      int v = st.top();
      st.pop();
      retg.resize(cnt);
      used.resize(cnt);
      if(!st.empty()){
        if(!used[m[v]] && v != st.top()) {
          retg[m[st.top()]].push_back({m[v],depth[v] - depth[st.top()]});
          used[m[v]] = 1;
        }
      }
      else retr = m[v];
    }
    vector<long long> ind(cnt);
    for(auto[i,j]:m) ind[j] = i;
    return {retg,ind,retr};
  }
};

}  // namespace soryuusi
