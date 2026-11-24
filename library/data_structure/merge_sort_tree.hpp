// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/merge_sort_tree.hpp
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

template<class Key, class Compare = less<Key>>
struct MergeSortTree{
  int n = 0, b = 0, base = 1;

  vector<Key> key;
  vector<Key> domain;

  MergeSortTree() = default;
  MergeSortTree(const vector<Key>& a) { build(a); }

  void build(const vector<Key>& a) {
    n = (int)a.size();
    b = n ? soryuusi::bit_width(static_cast<unsigned>(n-1))+1 : 1;
    base = 1 << (b-1);
    ranks.build(n,b-1);
    key.resize(b*n);
    for(int i = 0; i < n; ++i) key[(b-1)*n + i] = a[i];
    for(int j = 0; j < b-1; ++j){
      int start = (b-j-2)*n, k = start;
      int rmx = (b-j-1)*n;
      int mx = (b-j)*n;
      for(int i = 0; (i << j) < n; i += 2){
        int l = rmx;
        int r = min(l + (1 << j), mx);
        int lmx = r;
        rmx = min(r + (1 << j), mx);
        while(l < lmx || r < rmx){
          if(r == rmx || (l < lmx && Compare()(key[l],key[r]))) key[k++] = key[l++];
          else ranks.set_right(j,k-start), key[k++] = key[r++];
        }
      }
      ranks.finish(j);
    }
    domain.clear();
    domain_end.clear();
    for(int i = 0; i < n;) {
      int first = i++;
      while(i < n && !Compare()(key[first],key[i])) ++i;
      domain.push_back(key[first]);
      domain_end.push_back(i);
    }
  }

  int count_leq(int l, int r, Key x) const {
    assert(0 <= l && l <= r && r <= n);
    return count_bound<true>(l,r,x);
  }
  int count_lt(int l, int r, Key x) const {
    assert(0 <= l && l <= r && r <= n);
    return count_bound<false>(l,r,x);
  }

  Key kth_smallest(int l, int r, int k) const {
    assert(0 <= l && l <= r && r <= n);
    assert(1 <= k && k <= r - l);
    int lo = 0, hi = (int)domain.size() - 1;
    while (lo < hi) {
      int mid = (lo + hi) >> 1;
      int cnt = count_bound<true>(l,r,domain[mid],domain_end[mid]);
      if (cnt >= k) hi = mid;
      else lo = mid + 1;
    }
    return domain[lo];
  }

private:
  internal::MergeSortTreeRank ranks;
  vector<int> domain_end;

  template<bool Inclusive>
  int count_bound(int l, int r, const Key& x, int root_end = -1) const {
    auto bounds = [&](int a,int b) {
      if(root_end >= 0 && a == 0 && b == n) return pair{a,root_end};
      auto begin = key.begin();
      if constexpr(Inclusive) return pair{a,static_cast<int>(upper_bound(begin+a,begin+b,x,Compare())-begin)};
      else return pair{a,static_cast<int>(lower_bound(begin+a,begin+b,x,Compare())-begin)};
    };
    if(l == 0 && r == n) return bounds(0,n).second;
    int result = 0;
    ranks.visit(n,l,r,bounds,[&](int a,int b){ result += b-a; });
    return result;
  }
};

}  // namespace soryuusi
