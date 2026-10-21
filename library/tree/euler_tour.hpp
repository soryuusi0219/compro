// https://github.com/soryuusi0219/compro/blob/main/library/tree/euler_tour.hpp
#pragma once
#include <algorithm>
#include <type_traits>
#include <utility>
#include <vector>

#include "library/data_structure/segtree.hpp"
#include "library/graph/static_graph.hpp"

namespace soryuusi {

using namespace std;

template<class S, auto op, auto e>
struct EularTour{
  static_assert(is_invocable_r_v<S, decltype(op), S, S>, "op must be callable with (S, S) and return a value convertible to S");
  static_assert(is_invocable_r_v<S, decltype(e)>, "e must be callable with no arguments and return a value convertible to S");

  static constexpr int inf = 1ll << 30;
  struct StaticRMQ{
    static constexpr int b = 3;
    static constexpr int B = 1 << b;
    int n, m, h;
    vector<pair<int,int>> data;
    vector<pair<int,int>> left_prod;
    vector<pair<int,int>> right_prod;
    vector<vector<pair<int,int>>> sparse_table;
    StaticRMQ(){}
    StaticRMQ(vector<pair<int,int>> v){
      data = v;
      n = data.size();
      if(n <= B) return;
      m = ((n-1) >> b) + 1;
      left_prod = vector<pair<int,int>>(n+1,{inf,-1});
      right_prod = vector<pair<int,int>>(n+1,{inf,-1});
      h = 31-__builtin_clz(m);
      sparse_table = vector<vector<pair<int,int>>>(h+1, vector<pair<int,int>>(m+1,{inf,-1}));
      pair<int,int> temp = {inf,-1};
      for(int i = 0; i < n; i++){
        left_prod[i] = temp;
        temp = min(temp, data[i]);
        if(!(~i&(B-1))) {
          sparse_table[0][i >> b] = temp;
          temp = {inf,-1};
        }
      }
      left_prod[n] = temp;
      if(n&(B-1)){
        sparse_table[0][m-1] = temp;
        temp = {inf,-1};
      }
      for(int i = (n&~(B-1))-1; i >= 0; i--){
        temp = min(temp, data[i]);
        right_prod[i] = temp;
        if(!(i&(B-1))) {
          temp = {inf,-1};
        }
      }
      sparse_table[0][m] = sparse_table[0][m-1];
      for(int h0 = 1; h0 <= h; h0++){
        int i = 1 << h0;
        for(int j = i; j <= m; j += i << 1){
          temp = {inf,-1};
          for(int k = j-1; k >= j - i; k--){
            temp = min(sparse_table[0][k], temp);
            sparse_table[h0][k] = temp;
          }
          temp = {inf,-1};
          for(int k = j; k < min(j + i,m+1); k++){
            sparse_table[h0][k] = temp;
            temp = min(temp, sparse_table[0][k]);
          }
        }
      }
      for(int i = 1; i <= m; i += 2) sparse_table[0][i] = {inf,-1};
    }
    pair<int,int> naive_prod(int l, int r){
      pair<int,int> res = {inf,-1};
      for(int i = l; i < r; i++) res = min(res,data[i]);
      return res;
    }
    pair<int,int> prod(int l, int r){
      if(l >= r) return {inf,-1};
      if(r-l <= 8) return naive_prod(l,r);
      int l1 = (l >> b) + 1;
      int r1 = r >> b;
      if(l1 > r1) return naive_prod(l,r);
      if(l1 == r1) return min(right_prod[l], left_prod[r]);
      int mid = 31-__builtin_clz(l1^r1);
      return  min(min(right_prod[l],min(sparse_table[mid][l1], sparse_table[mid][r1])), left_prod[r]);
    }
  };
  int N = 0;
  bool is_built = false;
  static_graph<> G;
  vector<int> invertex, outvertex, tourid;
  vector<int> inseg, outseg;
  vector<S> data;
  StaticRMQ depth;
  Segtree<S,op,e> seg;
  EularTour(int n) : N(n), G(N,2*N-2) {
    invertex.resize(N);
    outvertex.resize(N);
    tourid.resize(2*N-1);
    data.resize(N);
    inseg.resize(N);
    outseg.resize(N);
  }
  EularTour(const vector<S>& a) : N(a.size()), G(N,2*N-2), data(a){
    invertex.resize(N);
    outvertex.resize(N);
    tourid.resize(2*N-1);
    inseg.resize(N);
    outseg.resize(N);
  }
  void add_edge(int u, int v){
    G.add_edge(u,v,true);
  }
  void build(int root = 0){
    G.build();
    int i = 0, j = 0;
    vector<pair<int,int>> depvec(2*N-1, {inf,-1});
    vector<S> datavec(N, e());
    auto dfs = [&](auto&& self, int v, int p, int dep) -> void {
      invertex[v] = i;
      inseg[v] = j;
      depvec[i] = {dep,v};
      tourid[i] = v; i++;
      datavec[j] = data[v]; j++;
      for(int c : G.vertices(v)) if(c != p){
        self(self,c,v,dep+1);
        depvec[i] = {dep,v};
        tourid[i] = v; i++;
      }
      outvertex[v] = i;
      outseg[v] = j;
    };
    dfs(dfs,root,-1,0);
    depth = StaticRMQ(depvec);
    seg = Segtree<S,op,e>(datavec);
    is_built = true;
  }
  int get_depth(int u){
    return depth.data[invertex[u]].first;
  }
  int lca(int u, int v){
    if(invertex[u] > outvertex[v]) swap(u,v);
    return depth.prod(invertex[u], outvertex[v]).second;
  }
  int dist(int u, int v){
    int w = lca(u,v);
    return get_depth(u)+get_depth(v)-get_depth(w)*2;
  }
  void set(int v, S x){
    data[v] = x;
    if(is_built) seg.set(inseg[v],x);
  }
  S get(int v){
    return data[v];
  }
  S prod(int v, bool contains_root = true){
    return seg.prod(inseg[v] + !contains_root, outseg[v]);
  }
};

}  // namespace soryuusi
