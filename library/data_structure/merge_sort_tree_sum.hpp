// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/merge_sort_tree_sum.hpp
#pragma once
#include <algorithm>
#include <cassert>
#include <functional>
#include <utility>
#include <vector>

#include "../math/internal/internal_bit.hpp"
#include "internal/merge_sort_tree_rank.hpp"

namespace soryuusi {

using namespace std;

template<class Key, class S, class Compare = less<Key>>
struct MergeSortTree_Sum{
  int n = 0, b = 0, base = 1;

  vector<Key> key;
  vector<S> data;

  MergeSortTree_Sum() = default;
  MergeSortTree_Sum(const vector<Key>& a, const vector<S>& v) { build(a, v); }

  void build(const vector<Key>& a, const vector<S>& v) {
    n = (int)a.size();
    b = n ? soryuusi::bit_width(static_cast<unsigned>(n-1))+1 : 1;
    base = 1 << (b-1);
    ranks.build(n,b-1);
    key.resize(b*n);
    data.resize(b*n+1);
    data[0] = 0;
    for(int i = 0; i < n; ++i) {
      key[(b-1)*n + i] = a[i];
      data[(b-1)*n + i + 1] = v[i];
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
            data[off_to + k + 1] = data[off_from + l + 1];
            ++l;
          }else{
            ranks.set_right(j,k);
            key[off_to + k] = key[off_from + r];
            data[off_to + k + 1] = data[off_from + r + 1];
            ++r;
          }
          ++k;
        }
      }
      ranks.finish(j);
    }
    for(int i = 0; i < b*n; i++) data[i+1] += data[i];
  }

  S prod(int l, int r){
    assert(0 <= l && l <= r && r <= n);
    if(l == r) return S(0);
    return data[r+(b-1)*n] - data[l+(b-1)*n];
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
    if(l == 0 && r == n) return data[bound<Inclusive>(0,n,x)]-data[0];
    S result = 0;
    auto bounds = [&](int a,int b){ return pair{a,bound<Inclusive>(a,b,x)}; };
    ranks.visit(n,l,r,bounds,[&](int a,int b){ result += data[b]-data[a]; });
    return result;
  }
};

}  // namespace soryuusi
