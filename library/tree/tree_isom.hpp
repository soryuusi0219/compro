// https://github.com/soryuusi0219/compro/blob/main/library/tree/tree_isom.hpp
#pragma once
#include <algorithm>
#include <functional>
#include <map>
#include <type_traits>
#include <utility>
#include <vector>

namespace soryuusi {

using namespace std;

namespace tree_hash{
  map<vector<int>,int> hash = [](){
    map<vector<int>,int> ret;
    ret[vector<int>()]=0;
    return ret;
  }();
	vector<vector<int>> hashvec(1);
  vector<int> get_rooted_hashs(const vector<vector<long long>>& t, int r = 0){
    int n = t.size();
    vector<int> ret(n);
    function<int(int, int)> get_hash = [&](int v, int p){
      vector<int> re;
		  for(int c : t[v]) if(c != p) re.push_back(get_hash(c,v));
      sort(re.begin(),re.end());
		  if(!hash.count(re)){
		    hash[re]=hashvec.size();
		  	hashvec.push_back(re);
	  	}
      return (ret[v] = hash[re]);
    };
    get_hash(r,-1);
    return ret;
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
  int get_rooted_hash(const vector<vector<long long>>& t, int r = 0){return get_rooted_hashs(t,r)[r];}
}
bool is_rooted_tree_isom(const vector<vector<long long>>& t1, const vector<vector<long long>>& t2, int r1 = 0, int r2 = 0){
  return tree_hash::get_rooted_hash(t1,r1)==tree_hash::get_rooted_hash(t2,r2);
}
bool is_tree_isom(const vector<vector<long long>>& t1, const vector<vector<long long>>& t2){
  auto [c0, c1] = tree_hash::tree_get_centroids(t1);
  auto [d0, d1] = tree_hash::tree_get_centroids(t2);
  if((c0 == c1) != (d0 == d1)) return false;
  if(c0 == c1) return is_rooted_tree_isom(t1,t2,c0,d0);
  return (is_rooted_tree_isom(t1,t2,c0,d0) || is_rooted_tree_isom(t1,t2,c1,d0));
}

}  // namespace soryuusi
