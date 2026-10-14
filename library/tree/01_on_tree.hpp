// https://github.com/soryuusi0219/compro/blob/main/library/tree/01_on_tree.hpp
#pragma once
#include <algorithm>
#include <numeric>
#include <utility>
#include <vector>

#include "library/data_structure/dsu.hpp"

namespace soryuusi {

using namespace std;

template<class T>
pair<T, vector<int>> solve01onTree(int n, vector<long long> par, vector<T> c, vector<T> d){
  int b = __bit_ceil((unsigned)n);
  vector<int> v(b*2,-1);
  auto cmp = [&](int i, int j){return !~i||(~j&&(!d[j]||c[i]*d[j]<c[j]*d[i]));};
  auto upd = [&](int i){v[i]=v[i<<1|cmp(v[i<<1],v[i<<1|1])];};
  for(int i = 0; i < n; ++i) v[i+b] = i;
  for(int i = b-1; i; --i) upd(i);
  DSU uf(n);
  T ans = 0, d0 = 0;
  int fr0 = -1, bk0 = -1;
  vector<int> nxt(n,-1), fr(n,-1), bk(n,-1);
  iota(fr.begin(), fr.end(), 0);
  iota(bk.begin(), bk.end(), 0);
  for(int _ = 0; _ < n; ++_){
    int i = v[1], j = par[i];
    vector<int> res;
    if(~j && ~v[(j=uf.leader(j)) + b]){
      ans += c[i] * d[j];
      nxt[bk[j]] = fr[i];
      bk[j] = bk[i];
      fr[i] = fr[j];
      par[i] = par[j];
      if(uf.merge(i,j) == i) swap(i,j);
      c[j] += c[i];
      d[j] += d[i];
      for(v[i+=b]=-1;i>>=1;) upd(i);
      for(j+=b;j>>=1;) upd(j);
    }else{
      if(~fr0) nxt[bk0] = fr[i], bk0 = bk[i];
      else fr0 = fr[i], bk0 = bk[i];
      ans += c[i] * d0;
      d0 += d[i];
      for(v[i+=b]=-1;i>>=1;) upd(i);
    }
  }
  vector<int> res(n);
  for(int i = 0; i < n; ++i, fr0 = nxt[fr0]) res[i] = fr0;
  return {ans,res};
}

}  // namespace soryuusi
