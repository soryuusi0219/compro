// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/merge_sort_tree_sum.hpp
#pragma once
#include <algorithm>
#include <cassert>
#include <functional>
#include <vector>

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
    base = 1, b++; while(base < n) base <<= 1, b++;
    key.resize(b*n);
    data.resize(b*n+1);
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
            key[off_to + k] = key[off_from + r];
            data[off_to + k + 1] = data[off_from + r + 1];
            ++r;
          }
          ++k;
        }
      }
    }
    for(int i = 0; i < b*n; i++) data[i+1] += data[i];
  }

  S prod(int l, int r){
    assert(0 <= l && l <= r && r <= n);
    return data[r+(b-1)*n] - data[l+(b-1)*n];
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
        if(l & K) res += data[upper_bound(key.begin() + l + k, key.begin() + (l + K + k), x, Compare()) - key.begin()] - data[l + k], l += K;
        if(r & K) r-=K, res += data[upper_bound(key.begin() + r + k, key.begin() + (r + K + k), x, Compare()) - key.begin()] - data[r + k];
    }
    return res;
  }
  S sum_lt(int l, int r, Key x) const {
    assert(0 <= l && l <= r && r <= n);
    S res = 0;
    for(int k = (b-1)*n, K = 1; l < r; k -= n, K <<= 1) {
        if(l & K) res += data[lower_bound(key.begin() + l + k, key.begin() + (l + K + k), x, Compare()) - key.begin()] - data[l + k], l += K;
        if(r & K) r-=K, res += data[lower_bound(key.begin() + r + k, key.begin() + (r + K + k), x, Compare()) - key.begin()] - data[r + k];
    }
    return res;
  }
};

}  // namespace soryuusi
