// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/merge_sort_tree_segtree.hpp
#pragma once
#include <algorithm>
#include <cassert>
#include <functional>
#include <numeric>
#include <type_traits>
#include <utility>
#include <vector>

#include "library/data_structure/segtree.hpp"

namespace soryuusi {

using namespace std;

template<class Key, class S, auto op, auto e, class Compare = less<Key>>
struct MergeSortTree_Segtree{
  static_assert(is_invocable_r_v<S, decltype(op), S, S>, "op must be callable with (S, S) and return a value convertible to S");
  static_assert(is_invocable_r_v<S, decltype(e)>, "e must be callable with no arguments and return a value convertible to S");

  int n = 0, b = 0, base = 1;

  vector<Key> key;
  Segtree<S,op,e> data;
  vector<vector<int>> idx;

  MergeSortTree_Segtree() = default;
  MergeSortTree_Segtree(const vector<Key>& a) { build(a, vector<S>(a.size(),e())); }
  MergeSortTree_Segtree(const vector<Key>& a, const vector<S>& v) { build(a, v); }

  void build(const vector<Key>& a, const vector<S>& v) {
    n = (int)a.size();
    base = 1, b++; while(base < n) base <<= 1, b++;
    key.resize(b*n);
    vector<S> _data(b*n);
    idx.assign(n, vector<int>(b,0));
    vector<int> ids(n);
    iota(ids.begin(),ids.end(),0);
    for(int i = 0; i < n; ++i) {
      key[(b-1)*n + i] = a[i];
      _data[(b-1)*n + i] = v[i];
      idx[i][0] = (b-1)*n+i;
    }
    for(int j = 0; j < b-1; ++j){
      int off_from = (b-j-1)*n, off_to = (b-j-2)*n;
      int k = 0;
      vector<int> nids(n);
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
            key[off_to + k] = key[off_from + r];
            _data[off_to + k] = _data[off_from + r];
            nids[k] = ids[r];
            idx[ids[r]][j+1] = off_to + k;
            ++r;
          }
          ++k;
        }
      }
      ids.swap(nids);
    }
    data = Segtree<S,op,e>(_data);
  }
  
  void set(int i, S x){
    assert(0 <= i && i < n);
    for(auto j : idx[i]) data.set(j,x);
  }
  S get(int i){
    assert(0 <= i && i < n);
    return data.get(idx[i][0]);
  }
  S prod(int l, int r){
    assert(0 <= l && l <= r && r <= n);
    return data.prod(idx[l][0], idx[r][0]);
  }

  S prod_interval(int l, int r, S lo, S up, bool mn_inclusive = true, bool mx_inclusive = false){
    assert(0 <= l && l <= r && r <= n);
    S res = e();
    for(int k = (b-1)*n, K = 1; l < r; k -= n, K <<= 1) {
      if(l & K) {
        int mn = mn_inclusive
          ? lower_bound(key.begin() + l + k, key.begin() + (l + K + k), lo, Compare()) - key.begin()
          : upper_bound(key.begin() + l + k, key.begin() + (l + K + k), lo, Compare()) - key.begin();
        int mx = mx_inclusive
          ? upper_bound(key.begin() + l + k, key.begin() + (l + K + k), up, Compare()) - key.begin()
          : lower_bound(key.begin() + l + k, key.begin() + (l + K + k), up, Compare()) - key.begin();
        res = op(res, data.prod(mn,mx));
        l += K;
      }
      if(r & K) {
        r -= K;
        int mn = mn_inclusive
          ? lower_bound(key.begin() + r + k, key.begin() + (r + K + k), lo, Compare()) - key.begin()
          : upper_bound(key.begin() + r + k, key.begin() + (r + K + k), lo, Compare()) - key.begin();
        int mx = mx_inclusive
          ? upper_bound(key.begin() + r + k, key.begin() + (r + K + k), up, Compare()) - key.begin()
          : lower_bound(key.begin() + r + k, key.begin() + (r + K + k), up, Compare()) - key.begin();
        res = op(res, data.prod(mn,mx));
      }
    }
    return res;
  }
};

}  // namespace soryuusi
