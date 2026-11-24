// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/merge_sort_tree_bit.hpp
#pragma once
#include <algorithm>
#include <cassert>
#include <functional>
#include <numeric>
#include <utility>
#include <vector>

#include "../math/internal/internal_bit.hpp"
#include "internal/merge_sort_tree_rank.hpp"

#include "bit.hpp"

namespace soryuusi {

using namespace std;

template<class Key, class S, class Compare = less<Key>>
struct MergeSortTree_BIT{
  int n = 0, b = 0, base = 1;

  vector<Key> key;
  BIT<S> data;
  vector<vector<int>> idx;

  MergeSortTree_BIT() = default;
  MergeSortTree_BIT(const vector<Key>& a) { build(a, vector<S>(a.size(),0)); }
  MergeSortTree_BIT(const vector<Key>& a, const vector<S>& v) { build(a, v); }

  void build(const vector<Key>& a, const vector<S>& v) {
    n = (int)a.size();
    b = n ? soryuusi::bit_width(static_cast<unsigned>(n-1))+1 : 1;
    base = 1 << (b-1);
    ranks.build(n,b-1);
    key.resize(b*n);
    vector<S> _data(b*n);
    idx.assign(n, vector<int>(b,0));
    vector<int> ids(n), nids(n);
    iota(ids.begin(),ids.end(),0);
    for(int i = 0; i < n; ++i) {
      key[(b-1)*n + i] = a[i];
      _data[(b-1)*n + i] = v[i];
      idx[i][0] = (b-1)*n+i;
    }
    for(int j = 0; j < b-1; ++j){
      int off_from = (b-j-1)*n, off_to = (b-j-2)*n;
      int k = 0;

      for(int i = 0; (i << j) < n; i += 2){
        int l = i << j;
        int r = min((i+1) << j, n);
        int lmx = r;
        int rmx = min(((i+2) << j), n);
        while(l < lmx || r < rmx){
          if(r == rmx || (l < lmx && Compare()(key[off_from + l],key[off_from + r]))){
            key[off_to + k] = key[off_from + l];
            _data[off_to + k] = _data[off_from + l];
            nids[k] = ids[l];
            idx[ids[l]][j+1] = off_to + k;
            ++l;
          }else{
            ranks.set_right(j,k);
            key[off_to + k] = key[off_from + r];
            _data[off_to + k] = _data[off_from + r];
            nids[k] = ids[r];
            idx[ids[r]][j+1] = off_to + k;
            ++r;
          }
          ++k;
        }
      }
      ranks.finish(j);
      ids.swap(nids);
    }
    data = BIT(_data);
  }
  
  void add(int i, S x){
    assert(0 <= i && i < n);
    for(auto j : idx[i]) data.add(j,x);
  }
  S get(int i){
    assert(0 <= i && i < n);
    return data.sum(idx[i][0],idx[i][0]+1);
  }
  S sum(int l, int r){
    assert(0 <= l && l <= r && r <= n);
    if(l == r) return S(0);
    return data.sum((b-1)*n+l,(b-1)*n+r);
  }

  int count_leq(int l, int r, Key x) const {
    assert(0 <= l && l <= r && r <= n);
    return count_bound<true>(l,r,x);
  }
  int count_lt(int l, int r, Key x) const {
    assert(0 <= l && l <= r && r <= n);
    return count_bound<false>(l,r,x);
  }
  S sum_leq(int l, int r, Key x) const {
    assert(0 <= l && l <= r && r <= n);
    return sum_bound<true>(l,r,x);
  }
  S sum_lt(int l, int r, Key x) const {
    assert(0 <= l && l <= r && r <= n);
    return sum_bound<false>(l,r,x);
  }

private:
  internal::MergeSortTreeRank ranks;

  template<bool Inclusive>
  int bound(int a, int b, const Key& x) const {
    auto begin = key.begin();
    if constexpr(Inclusive) return upper_bound(begin+a,begin+b,x,Compare())-begin;
    else return lower_bound(begin+a,begin+b,x,Compare())-begin;
  }
  template<bool Inclusive>
  int count_bound(int l, int r, const Key& x) const {
    if(l == 0 && r == n) return bound<Inclusive>(0,n,x);
    int result = 0;
    auto bounds = [&](int a,int b){ return pair{a,bound<Inclusive>(a,b,x)}; };
    ranks.visit(n,l,r,bounds,[&](int a,int b){ result += b-a; });
    return result;
  }
  template<bool Inclusive>
  S sum_bound(int l, int r, const Key& x) const {
    if(l == r) return S(0);
    if(l == 0 && r == n) return data.sum(0,bound<Inclusive>(0,n,x));
    S result = 0;
    auto bounds = [&](int a,int b){ return pair{a,bound<Inclusive>(a,b,x)}; };
    ranks.visit(n,l,r,bounds,[&](int a,int b){ result += data.sum(a,b); });
    return result;
  }
};

}  // namespace soryuusi
