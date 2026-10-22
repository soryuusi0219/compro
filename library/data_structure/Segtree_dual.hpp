// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/Segtree_dual.hpp
#pragma once
#include <bit>
#include <type_traits>
#include <vector>
namespace soryuusi {

using namespace std;

template<class S, class F, auto mapping, auto composition, auto id>
struct Segtree_dual{
  static_assert(is_invocable_r_v<S, decltype(mapping), F, S>, "mapping must be callable with (F, S) and return a value convertible to S");
  static_assert(is_invocable_r_v<F, decltype(composition), F, F>, "composition must be callable with (F, F) and return a value convertible to F");
  static_assert(is_invocable_r_v<F, decltype(id)>, "id must be callable with no arguments and return a value convertible to F");

  private:
    int n, h, b;
    vector<S> d;
    vector<F> lz;
    void act(int k, F f) {
      if(k < b) lz[k] = composition(f, lz[k]);
      else if(k < b+n) d[k-b] = mapping(f, d[k-b]);
    }
    void push(int k) {
      act(k << 1, lz[k]), act(k << 1 | 1, lz[k]);
      lz[k] = id();
    }
    void prop(int l, int r) {
      int il = __builtin_ctz(l), ir = __builtin_ctz(r);
      for(int i = h; i > il; i--) push(l >> i);
      for(int i = h; i > ir; i--) push((r - 1) >> i);
    }
  public:
    Segtree_dual(){}
    Segtree_dual(int n_) : n(n_), h(bit_width(bit_ceil((unsigned)n))-1), b(bit_ceil((unsigned)n)), d(n), lz(b, id()){}
    Segtree_dual(int n_, S x) : n(n_), h(bit_width(bit_ceil((unsigned)n))-1), b(bit_ceil((unsigned)n)), d(n,x), lz(b, id()){}
    Segtree_dual(const vector<S>& v) : n(v.size()), h(bit_width(bit_ceil((unsigned)n))-1), b(bit_ceil((unsigned)n)), d(v), lz(b, id()){}
    void set(int i, S a) {
      for(int k = h; k; --k) push((i+b) >> k);
      d[i] = a;
    }
    S get(int i){
      for(int k = h; k; --k) push((i+b) >> k);
      return d[i];
    }
    void apply(int l, int r, F f) {
      if(l == r) return;
      l += b, r += b;
      prop(l, r);
      for(; l < r; l >>= 1, r >>= 1) {
        if(l & 1) act(l++, f);
        if(r & 1) act(--r, f);
      }
    }
    void all_apply(F f){act(1, f);}
};

}  // namespace soryuusi
