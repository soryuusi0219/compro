// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/range_parallel_unionfind.hpp
#pragma once
#include <algorithm>
#include <vector>

#include "library/math/internal_bit.hpp"
#include "library/data_structure/dsu.hpp"

namespace soryuusi {

using namespace std;

struct Range_Parallel_Unionfind{
  int n;
  int m;
  vector<DSU> d;
  Range_Parallel_Unionfind(int n_) : n(n_), m(bit_width((unsigned)n)), d(m, DSU(n)){}
  int leader(int u){return d[0].leader(u);}
  bool same(int u, int v){return d[0].same(u, v);}
  vector<vector<int>> groups(){return d[0].groups();} 
  void _merge(int l, int r, int i){
    if(!d[i].same(l,r)) {
      d[i].merge(l,r);
      if(i){
        _merge(l,r,i-1);
        _merge(l+(1 << (i-1)),r+(1 << (i-1)),i-1);
      }
    }
  }
  void merge(int l, int r, int x){
    for(int i=0; x; x>>=1, i++) if(x&1){
      _merge(l,r,i);
      l += (1 << i);
      r += (1 << i);
    }
  }
  template<class F>
  void _merge(int l, int r, int i, F f){
    if(!d[i].same(l,r)) {
      if(i){
        d[i].merge(l,r);
        _merge(l,r,i-1,f);
        _merge(l+(1 << (i-1)),r+(1 << (i-1)),i-1,f);
      }else {
        l = d[0].leader(l);
        r = d[0].leader(r);
        f(l,r,d[0].merge(l,r));
      }
    }
  }
  template<class F>
  void merge(int l, int r, int x, F f){
    for(int i=0; x; x>>=1, i++) if(x&1){
      _merge(l,r,i,f);
      l += (1 << i);
      r += (1 << i);
    }
  }
};

}  // namespace soryuusi
