// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/lazy_segtree.hpp
#pragma once
#include <type_traits>
#include <vector>

#include "../math/internal/internal_bit.hpp"
#include "internal/segtree_ops.hpp"

namespace soryuusi {

using namespace std;

template<class S, auto op, auto e, class F, auto mapping, auto composition, auto id>
struct LazySegtree{
  static_assert(is_invocable_r_v<S, decltype(op), S, S>, "op must be callable with (S, S) and return a value convertible to S");
  static_assert(is_invocable_r_v<S, decltype(e)>, "e must be callable with no arguments and return a value convertible to S");
  static_assert(is_invocable_r_v<S, decltype(mapping), F, S>, "mapping must be callable with (F, S) and return a value convertible to S");
  static_assert(is_invocable_r_v<F, decltype(composition), F, F>, "composition must be callable with (F, F) and return a value convertible to F");
  static_assert(is_invocable_r_v<F, decltype(id)>, "id must be callable with no arguments and return a value convertible to F");

  private:
    int n, b, h;
    vector<S> d;
    vector<F> lz;
    void upd(int k) { d[k] = op(d[k << 1], d[k << 1 | 1]); }
    void act(int k, F f) {
      d[k] = mapping(f, d[k]);
      if(k < b) lz[k] = composition(f, lz[k]);
    }
    void push(int k) {
      act(k << 1, lz[k]), act(k << 1 | 1, lz[k]);
      lz[k] = id();
    }
  public:
    LazySegtree(int n_ = 0) : n(n_), b(soryuusi::bit_ceil(static_cast<unsigned>(n))), h(soryuusi::countr_zero(static_cast<unsigned>(b))), d(2 * b, e()), lz(b, id()) {}
    LazySegtree(const vector<S>& v) : LazySegtree(static_cast<int>(v.size())) {
      for(int i = 0; i < n; i++) d[b + i] = v[i];
      for(int i = b - 1; i > 0; i--) upd(i);
    }
    void set(int p, S x) {
      p += b;
      internal::segtree_push_point(p, h, [&](int k){ push(k); });
      d[p] = x;
      internal::segtree_pull_point(p, [&](int k){ upd(k); });
    }
    void reset(int i){ set(i,e());}
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
    void apply(int p, F f) {
      p += b;
      internal::segtree_push_point(p, h, [&](int k){ push(k); });
      act(p, f);
      internal::segtree_pull_point(p, [&](int k){ upd(k); });
    }
    void apply(int l, int r, F f) {
      if(l == r) return;
      l += b, r += b;
      internal::SegtreeBoundary boundary(l, r, h);
      boundary.push([&](int k){ push(k); });
      for(; l < r; l >>= 1, r >>= 1) {
        if(l & 1) act(l++, f);
        if(r & 1) act(--r, f);
      }
      boundary.pull([&](int k){ upd(k); });
    }
    void all_apply(F f) { act(1,f); }
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
