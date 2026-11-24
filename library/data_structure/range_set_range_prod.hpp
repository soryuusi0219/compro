// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/range_set_range_prod.hpp
#pragma once
#include <type_traits>
#include <vector>

#include "../math/internal/internal_bit.hpp"
#include "internal/segtree_ops.hpp"

namespace soryuusi {

using namespace std;

template<class S, auto op, auto e>
struct RangeSetRangeProd{
  static_assert(is_invocable_r_v<S, decltype(op), S, S>, "op must be callable with (S, S) and return a value convertible to S");
  static_assert(is_invocable_r_v<S, decltype(e)>, "e must be callable with no arguments and return a value convertible to S");

  private:
    int n, b, h;
    vector<S> d;
    vector<int> lz;
    vector<S> lzval;
    void upd(int k) { d[k] = op(d[k << 1], d[k << 1 | 1]); }
    void act(int k, int f) {
      if(~f){
        d[k] = lzval[f];
        if(k < b) lz[k] = f-1;
      }
    }
    void push(int k) {
      if(~lz[k]){
        act(k << 1, lz[k]), act(k << 1 | 1, lz[k]);
        lz[k] = -1;
      }
    }
    void all_reduce(){
      for(int i = 1; i < b; i++) push(i);
      lzval.clear();
    }

  public:
    RangeSetRangeProd(int n_ = 0) : n(n_), b(soryuusi::bit_ceil(static_cast<unsigned>(n))), h(soryuusi::countr_zero(static_cast<unsigned>(b))), d(2 * b, e()), lz(b, -1) {}
    RangeSetRangeProd(const vector<S>& v) : RangeSetRangeProd(static_cast<int>(v.size())) {
      for(int i = 0; i < n; i++) d[b + i] = v[i];
      for(int i = b - 1; i > 0; i--) upd(i);
    }
    void set(int p, S x) {
      p += b;
      internal::segtree_push_point(p, h, [&](int k){ push(k); });
      d[p] = x;
      internal::segtree_pull_point(p, [&](int k){ upd(k); });
    }
    S get(int p) {
      p += b;
      internal::segtree_push_point(p, h, [&](int k){ push(k); });
      return d[p];
    }
    S prod(int l, int r) {
      if(l == r) return e();
      l += b, r += b;
      internal::SegtreeBoundary(l, r, h).push([&](int k){ push(k); });
      S left = e(), right = e();
      for(; l < r; l >>= 1, r >>= 1) {
        if(l & 1) left = op(left,d[l++]);
        if(r & 1) right = op(d[--r],right);
      }
      return op(left,right);
    }
    S all_prod() { return d[1]; }
    void range_set(int l, int r, S x) {
      if(l == r) return;
      if(r-l == 1) {
        set(l,x);
        return;
      }
      // The new tags cover every old tag before it can be read.
      bool all = l == 0 && r == n;
      l += b, r += b;
      internal::SegtreeBoundary boundary(l, r, h);
      if(all) lzval.clear();
      else boundary.push([&](int k){ push(k); });
      for(int f = lzval.size();; ++f) {
        lzval.push_back(x);
        if(l & 1) act(l++, f);
        if(r & 1) act(--r, f);
        l >>= 1, r >>= 1;
        if(l >= r) break;
        x = op(x,x);
      }
      boundary.pull([&](int k){ upd(k); });
      if((int)lzval.size() > n*2) all_reduce();
    }
    int max_right(int l, auto g) {
      if(l == n) return n;
      l += b;
      internal::segtree_push_point(l, h, [&](int k){ push(k); });
      S sm = e();
      do {
        while(~l & 1) l >>= 1;
        if(!g(op(sm, d[l]))) {
            while(l < b) {
              push(l);
              l <<= 1;
              if(g(op(sm, d[l]))) sm = op(sm, d[l++]);
            }
            return l - b;
        }
        sm = op(sm, d[l++]);
      } while((l & -l) != l);
      return n;
    }
    int min_left(int r, auto g) {
      if(r == 0) return 0;
      r += b;
      internal::segtree_push_point(r - 1, h, [&](int k){ push(k); });
      S sm = e();
      do {
        r--;
        while(r > 1 && (r & 1)) r >>= 1;
        if(!g(op(d[r], sm))) {
            while(r < b) {
              push(r);
              r = r << 1 | 1;
              if(g(op(d[r], sm))) sm = op(d[r--], sm);
            }
            return r + 1 - b;
        }
        sm = op(d[r], sm);
      } while((r & -r) != r);
      return 0;
    }
};

}  // namespace soryuusi
