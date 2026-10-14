// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/merge_sort_tree_bit.hpp
#pragma once
#include <algorithm>
#include <cassert>
#include <functional>
#include <numeric>
#include <utility>
#include <vector>

#include "library/data_structure/bit.hpp"

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
    return data.sum(idx[l][0], idx[r][0]);
  }

  int count_leq(int l, int r, Key x) const {
    assert(0 <= l && l <= r && r <= n);
    int res = 0;
    for(int k = (b-1)*n, K = 1; l < r; k -= n, K <<= 1) {
        if(l & K) res += (int)(upper_bound(key.begin() + l + k, key.begin() + (l + K + k), x, Compare()) - (key.begin() + l + k)), l += K;
        if(r & K) r-=K, res += (int)(upper_bound(key.begin() + r + k, key.begin() + (r + K + k), x, Compare()) - (key.begin() + r + k));
    }
    return res;
  }
  int count_lt(int l, int r, Key x) const {
    assert(0 <= l && l <= r && r <= n);
    int res = 0;
    for(int k = (b-1)*n, K = 1; l < r; k -= n, K <<= 1) {
        if(l & K) res += (int)(lower_bound(key.begin() + l + k, key.begin() + (l + K + k), x, Compare()) - (key.begin() + l + k)), l += K;
        if(r & K) r-=K, res += (int)(lower_bound(key.begin() + r + k, key.begin() + (r + K + k), x, Compare()) - (key.begin() + r + k));
    }
    return res;
  }

  S sum_leq(int l, int r, Key x) const {
    assert(0 <= l && l <= r && r <= n);
    S res = 0;
    for(int k = (b-1)*n, K = 1; l < r; k -= n, K <<= 1) {
        if(l & K) res += data.sum(l + k, upper_bound(key.begin() + l + k, key.begin() + (l + K + k), x, Compare()) - key.begin()), l += K;
        if(r & K) r -= K, res += data.sum(r + k, upper_bound(key.begin() + r + k, key.begin() + (r + K + k), x, Compare()) - key.begin());
    }
    return res;
  }
  S sum_lt(int l, int r, Key x) const {
    assert(0 <= l && l <= r && r <= n);
    S res = 0;
    for(int k = (b-1)*n, K = 1; l < r; k -= n, K <<= 1) {
        if(l & K) res += data.sum(l + k, lower_bound(key.begin() + l + k, key.begin() + (l + K + k), x, Compare()) - key.begin()), l += K;
        if(r & K) r -= K, res += data.sum(r + k, lower_bound(key.begin() + r + k, key.begin() + (r + K + k), x, Compare()) - key.begin());
    }
    return res;
  }
};

}  // namespace soryuusi
