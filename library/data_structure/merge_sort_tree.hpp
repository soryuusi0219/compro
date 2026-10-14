// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/merge_sort_tree.hpp
#pragma once
#include <algorithm>
#include <cassert>
#include <functional>
#include <vector>

namespace soryuusi {

using namespace std;

template<class Key, class Compare = less<Key>>
struct MergeSortTree{
  int n = 0, b = 0, base = 1;

  vector<Key> key;
  vector<Key> domain;

  MergeSortTree() = default;
  MergeSortTree(const vector<Key>& a) { build(a); }

  void build(const vector<Key>& a) {
    n = (int)a.size();
    base = 1, b++; while(base < n) base <<= 1, b++;
    key.resize(b*n);
    for(int i = 0; i < n; ++i) key[(b-1)*n + i] = a[i];
    for(int j = 0; j < b-1; ++j){
      int k = (b-j-2)*n;
      int rmx = (b-j-1)*n;
      int mx = (b-j)*n;
      for(int i = 0; (i << j) < n; i += 2){
        int l = rmx;
        int r = min(l + (1 << j), mx);
        int lmx = r;
        rmx = min(r + (1 << j), mx);
        while(l < lmx || r < rmx){
          if(r == rmx || (l < lmx && Compare()(key[l],key[r]))) key[k++] = key[l++];
          else key[k++] = key[r++];
        }
      }
    }
    domain.assign(a.begin(), a.end());
    sort(domain.begin(), domain.end());
    domain.erase(unique(domain.begin(), domain.end()), domain.end());
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

  Key kth_smallest(int l, int r, int k) const {
    assert(0 <= l && l <= r && r <= n);
    assert(1 <= k && k <= r - l);
    int lo = 0, hi = (int)domain.size() - 1;
    while (lo < hi) {
      int mid = (lo + hi) >> 1;
      int cnt = count_leq(l, r, domain[mid]);
      if (cnt >= k) hi = mid;
      else lo = mid + 1;
    }
    return domain[lo];
  }
};

}  // namespace soryuusi
