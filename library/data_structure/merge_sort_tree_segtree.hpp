// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/merge_sort_tree_segtree.hpp
#pragma once
#include <algorithm>
#include <cassert>
#include <functional>
#include <numeric>
#include <type_traits>
#include <utility>
#include <vector>

#include "../math/internal/internal_bit.hpp"
#include "internal/merge_sort_tree_rank.hpp"

#include "segtree.hpp"

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
    data = Segtree<S,op,e>(std::move(_data));
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
    if(l == r) return e();
    return data.prod((b-1)*n+l,(b-1)*n+r);
  }

  S prod_interval(int l, int r, S lo, S up, bool mn_inclusive = true, bool mx_inclusive = false){
    assert(0 <= l && l <= r && r <= n);
    auto bounds = [&](int a,int b) {
      auto begin = key.begin();
      int first = (mn_inclusive ? lower_bound(begin+a,begin+b,lo,Compare()) : upper_bound(begin+a,begin+b,lo,Compare()))-begin;
      int last = (mx_inclusive ? upper_bound(begin+a,begin+b,up,Compare()) : lower_bound(begin+a,begin+b,up,Compare()))-begin;
      return pair{first,last};
    };
    S res = e();
    ranks.visit(n,l,r,bounds,[&](int a,int b){ res = op(res,data.prod(a,b)); });
    return res;
  }

private:
  internal::MergeSortTreeRank ranks;
};

}  // namespace soryuusi
