// https://github.com/soryuusi0219/compro/blob/main/library/tree/tree_isom.hpp
#pragma once
#include <algorithm>
#include <map>
#include <utility>
#include <vector>

namespace soryuusi {

using namespace std;

namespace tree_hash{

  inline map<vector<int>,int> hash{{{},0}};
  inline vector<vector<int>> hashvec(1);

  inline vector<int> get_rooted_hashs(const vector<vector<long long>>& t, int r = 0){
    vector<int> ret(t.size()), children;
    struct Frame {int vertex, next;};
    vector<Frame> stack{{r,0}};
    while(!stack.empty()){
      auto& [v,next] = stack.back();
      const int parent = stack.size() == 1 ? -1 : stack[stack.size()-2].vertex;
      if(next < (int)t[v].size()){
        const int c = t[v][next++];
        if(c != parent) stack.push_back({c,0});
        continue;
      }
      for(int c : t[v]) if(c != parent) children.push_back(ret[c]);
      if(children.size() > 1 && !is_sorted(children.begin(),children.end()))
        sort(children.begin(),children.end());
      const auto [it,inserted] = hash.try_emplace(children,hashvec.size());
      if(inserted) hashvec.push_back(children);
      ret[v] = it->second;
      children.clear();
      stack.pop_back();
    }
    return ret;
  }

  inline pair<long long, long long> tree_get_centroids(const vector<vector<long long>>& G){
    const int n = G.size();
    struct Frame {int vertex, next, size;};
    vector<Frame> stack{{0,0,1}};
    while(!stack.empty()){
      auto& [v,next,size] = stack.back();
      const int p = stack.size() == 1 ? -1 : stack[stack.size()-2].vertex;
      if(next < (int)G[v].size()){
        const int c = G[v][next++];
        if(c != p) stack.push_back({c,0,1});
        continue;
      }
      if(size >= n-size){
        if(size == n-size) return {min(v,p),max(v,p)};
        return {v,v};
      }
      const int subtree = size;
      stack.pop_back();
      stack.back().size += subtree;
    }
    return {-1,-1};
  }

  inline int get_rooted_hash(const vector<vector<long long>>& t, int r = 0){return get_rooted_hashs(t,r)[r];}
}

inline bool is_rooted_tree_isom(const vector<vector<long long>>& t1, const vector<vector<long long>>& t2, int r1 = 0, int r2 = 0){
  return t1.size() == t2.size() && tree_hash::get_rooted_hash(t1,r1) == tree_hash::get_rooted_hash(t2,r2);
}
inline bool is_tree_isom(const vector<vector<long long>>& t1, const vector<vector<long long>>& t2){
  if(t1.size() != t2.size()) return false;
  auto [c0, c1] = tree_hash::tree_get_centroids(t1);
  auto [d0, d1] = tree_hash::tree_get_centroids(t2);
  if((c0 == c1) != (d0 == d1)) return false;
  if(c0 == c1) return is_rooted_tree_isom(t1,t2,c0,d0);
  return is_rooted_tree_isom(t1,t2,c0,d0) || is_rooted_tree_isom(t1,t2,c1,d0);
}

}  // namespace soryuusi
