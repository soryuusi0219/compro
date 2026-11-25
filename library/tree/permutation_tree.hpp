// https://github.com/soryuusi0219/compro/blob/main/library/tree/permutation_tree.hpp
#pragma once

#include <vector>

#include "../data_structure/static_rmq.hpp"

namespace soryuusi{

using namespace std;

struct PermutationTree{
  using StaticRMQ = soryuusi::StaticRMQ<int>;
  enum NodeType {
    Prime,
    Dec,
    Inc,
    One
  };
  struct Node {
    int p, l, r;
    NodeType type;
  };
  vector<Node> tree;

  PermutationTree() = default;
  template<class INT> PermutationTree(const vector<INT>& p) {build(p);}
  template<class INT> void build(const vector<INT>& p) {
    tree.clear();
    int n = p.size();
    vector<int> lower(n), upper(n);
    for(int i = 0; i < n; ++i) lower[i] = i-1, upper[i] = i+1;
    // Removing values backwards leaves exactly the values in each prefix.
    // Once removed, a value retains its nearest neighbors in that prefix.
    for(int i = n-1; i >= 0; --i){
      const int v = p[i], lo = lower[v], hi = upper[v];
      if(lo >= 0) upper[lo] = hi;
      if(hi < n) lower[hi] = lo;
    }
    struct Data{int l, d, u;};
    vector<Data> st;
    vector<int> ids(n);
    for(int i = 0; i < n; ++i) {
      tree.emplace_back(-1,i,i+1,One);
      ids[i] = i;
    }
    int k = n;
    for(int r = 1; r <= n; ++r){
      int y = r - 1;
      int d = p[y], u = p[y] + 1;
      int pd = p[y], pu = p[y] + 1;
      while(st.size()){
        auto& [x, xd, xu] = st.back();
        if(xd > pd) xd = pd;
        if(xu < pu) xu = pu;
        if(r-x == xu-xd) {
          if(tree[ids[x]].r != y){
            tree.emplace_back(-1,x,r,Prime);
            for(int z = x; z < r; z = tree[ids[z]].r) tree[ids[z]].p = k;
            ids[x] = k++;
          }else {
            NodeType t = p[x] < p[y] ? Inc : Dec;
            if(tree[ids[x]].type == t) tree[ids[x]].r = r, tree[ids[y]].p = ids[x];
            else{
              tree.emplace_back(-1,x,r,t);
              tree[ids[x]].p = k, tree[ids[y]].p = k;
              ids[x] = k++;
            }
          }
          y = x, d = xd, u = xu;
        }
        // p[x] lies in [xd,xu). The two neighbors detect any earlier value in it.
        else if(lower[p[x]] < xd && xu <= upper[p[x]]) break;
        pd = xd, pu = xu;
        st.pop_back();
      }
      st.emplace_back(y,d,u);
    }
  }
};

} //namespace soryuusi
