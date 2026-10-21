// https://github.com/soryuusi0219/compro/blob/main/library/tree/auxiliary_tree.hpp
#pragma once
#include <algorithm>
#include <tuple>
#include <utility>
#include <vector>

#include "library/tree/simple_euler_tour.hpp"

namespace soryuusi {

using namespace std;

struct Auxiliary_Tree : SimpleEularTour{
  int root;
  Auxiliary_Tree(int n, int r = 0) : SimpleEularTour(n), root(r) {}
  void set_up(){build(root);}
  int up(int v, int k){
    int d = get_depth(v);
    if(k >= d) return root;
    int target = d-k, l = 0, r = invertex[v]+1;
    while(r-l > 1){
      int m = (l+r) >> 1;
      if(depth.prod(m,invertex[v]+1).first <= target) l = m;
      else r = m;
    }
    return tourid[l];
  }

  pair<vector<int>,vector<int>> auxiliary_tree_parent(vector<int> s){
    if(s.empty()) return {{},{}};
    sort(s.begin(),s.end(),[&](int x, int y){return invertex[x] < invertex[y];});
    s.erase(unique(s.begin(),s.end()),s.end());
    int k = s.size();
    s.reserve(k*2-1);
    for(int i = 1; i < k; i++) s.push_back(lca(s[i-1],s[i]));
    sort(s.begin(),s.end(),[&](int x, int y){return invertex[x] < invertex[y];});
    s.erase(unique(s.begin(),s.end()),s.end());
    vector<int> par(s.size(),-1), st;
    st.reserve(s.size());
    for(int i = 0; i < (int)s.size(); i++){
      while(!st.empty() && outvertex[s[st.back()]] < invertex[s[i]]) st.pop_back();
      if(!st.empty()) par[i] = st.back();
      st.push_back(i);
    }
    return {s,par};
  }

  tuple<vector<vector<int>>,vector<int>,int> auxiliary_tree(vector<int> s){
    auto [ind,par] = auxiliary_tree_parent(s);
    vector<vector<int>> retg(ind.size());
    for(int i = 1; i < (int)ind.size(); i++) retg[par[i]].push_back(i);
    return {retg,ind,ind.empty() ? -1 : 0};
  }

  tuple<vector<vector<pair<int,int>>>,vector<int>,int> auxiliary_tree_dist(vector<int> s){
    auto [ind,par] = auxiliary_tree_parent(s);
    vector<vector<pair<int,int>>> retg(ind.size());
    for(int i = 1; i < (int)ind.size(); i++) retg[par[i]].push_back({i,get_depth(ind[i])-get_depth(ind[par[i]])});
    return {retg,ind,ind.empty() ? -1 : 0};
  }
};

}  // namespace soryuusi
