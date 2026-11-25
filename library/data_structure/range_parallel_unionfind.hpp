// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/range_parallel_unionfind.hpp
#pragma once
#include <vector>

#include "../math/internal/internal_bit.hpp"
#include "dsu.hpp"

namespace soryuusi {

using namespace std;

struct Range_Parallel_Unionfind{
  int n;
  int m;
  vector<DSU> d;
  Range_Parallel_Unionfind(int n_) : n(n_), m(soryuusi::bit_width((unsigned)n)) {
    d.reserve(m);
    for(int i = 0; i < m; ++i) d.emplace_back(n);
  }
  int leader(int u){return d[0].leader(u);}
  bool same(int u, int v){return d[0].same(u, v);}
  vector<vector<int>> groups(){return n == 0 ? vector<vector<int>>() : d[0].groups();}
  void _merge(int l, int r, int i){
    _merge(l,r,i,[](int,int,int){});
  }
  void merge(int l, int r, int x){
    merge(l,r,x,[](int,int,int){});
  }
  template<class F>
  void _merge(int l, int r, int i, F f){
    int a = d[i].leader(l), b = d[i].leader(r);
    if(a == b) return;
    if(i == 0) {
      f(a,b,d[0].merge_roots(a,b));
      return;
    }
    d[i].merge_roots(a,b);
    int half = 1 << (i-1);
    _merge(l,r,i-1,f);
    _merge(l+half,r+half,i-1,f);
  }
  template<class F>
  void merge(int l, int r, int x, F f){
    for(; x; x &= x-1) {
      int i = soryuusi::countr_zero(static_cast<unsigned>(x));
      _merge(l,r,i,f);
      l += 1 << i;
      r += 1 << i;
    }
  }
};

}  // namespace soryuusi
