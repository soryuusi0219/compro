// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/segtree_dual.hpp
#pragma once
#include <type_traits>
#include <vector>

#include "../math/internal/internal_bit.hpp"
#include "internal/segtree_ops.hpp"
namespace soryuusi {

using namespace std;

template<class S, class F, auto mapping, auto composition, auto id>
struct Segtree_dual{
  static_assert(is_invocable_r_v<S, decltype(mapping), F, S>, "mapping must be callable with (F, S) and return a value convertible to S");
  static_assert(is_invocable_r_v<F, decltype(composition), F, F>, "composition must be callable with (F, F) and return a value convertible to F");
  static_assert(is_invocable_r_v<F, decltype(id)>, "id must be callable with no arguments and return a value convertible to F");

  private:
    int n, b, h;
    vector<S> d;
    internal::SegtreeTags<F,composition,id> lz;
    void act(int k, F f) {
      if(k < b) lz.apply(k,f);
      else if(k < b+n) d[k-b] = mapping(f, d[k-b]);
    }
    void push(int k) {
      lz.push(k, [&](int i, auto&& f){ act(i,f); });
    }
  public:
    Segtree_dual(){}
    Segtree_dual(int n_) : n(n_), b(soryuusi::bit_ceil(static_cast<unsigned>(n))), h(soryuusi::countr_zero(static_cast<unsigned>(b))), d(n), lz(b){}
    Segtree_dual(int n_, S x) : n(n_), b(soryuusi::bit_ceil(static_cast<unsigned>(n))), h(soryuusi::countr_zero(static_cast<unsigned>(b))), d(n,x), lz(b){}
    Segtree_dual(const vector<S>& v) : n(v.size()), b(soryuusi::bit_ceil(static_cast<unsigned>(n))), h(soryuusi::countr_zero(static_cast<unsigned>(b))), d(v), lz(b){}
    void set(int i, S a) {
      internal::segtree_push_point(i+b, h, [&](int k){ push(k); });
      d[i] = a;
    }
    S get(int i){
      internal::segtree_push_point(i+b, h, [&](int k){ push(k); });
      return d[i];
    }
    void apply(int l, int r, F f) {
      if(l == r) return;
      l += b, r += b;
      internal::SegtreeBoundary(l, r, h).push([&](int k){ push(k); });
      for(; l < r; l >>= 1, r >>= 1) {
        if(l & 1) act(l++, f);
        if(r & 1) act(--r, f);
      }
    }
    void all_apply(F f){act(1, f);}
};

}  // namespace soryuusi
