// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/segtree_beats.hpp
#pragma once
#include <bit>
#include <vector>

namespace soryuusi {

using namespace std;

template<class S, S op(S,S), S e(), class F, S mapping(F,S), F composition(F,F), F id()> 
struct segtree_beats{
  private:
    int n, b, h;
    vector<S> d;
    vector<F> lz;
    void upd(int k) { d[k] = op(d[k << 1], d[k << 1 | 1]); }
    void act(int k, F f) {
      d[k] = mapping(f, d[k]);
      if(k < b) {
        lz[k] = composition(f, lz[k]);
        if(d[k].fail) push(k), upd(k);
      }
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
    void build(int l, int r) {
      int il = __builtin_ctz(l), ir = __builtin_ctz(r);
      for(int i = il + 1; i <= h; i++) upd(l >> i);
      for(int i = ir + 1; i <= h; i++) upd((r - 1) >> i);
    }

  public:
    segtree_beats(int n_ = 0) : segtree_beats(vector<S>(n_, e())) {}
    segtree_beats(const vector<S>& v) : n((int)v.size()) {
      h = bit_width((unsigned)n), b = 1 << h;
      d.assign(2 * b, e()), lz.assign(b, id());
      for(int i = 0; i < n; i++) d[b + i] = v[i];
      for(int i = b - 1; i > 0; i--) upd(i);
    }
    void set(int p, S x) {
      p += b;
      prop(p, p + 1);
      d[p] = x;
      build(p, p + 1);
    }
    S get(int p) {
      p += b;
      prop(p, p + 1);
      return d[p];
    }
    S prod(int l, int r) {
      if(l == r) return e();
      prop(l += b, r += b);
      S sl = e(), sr = e();
      for(; l < r; l >>= 1, r >>= 1) {
        if(l & 1) sl = op(sl, d[l++]);
        if(r & 1) sr = op(d[--r], sr);
      }
      return op(sl, sr);
    }
    S all_prod() { return d[1]; }
    void apply(int p, F f) {
      p += b;
      prop(p, p + 1);
      act(p, f);
      build(p, p + 1);
    }
    void apply(int l, int r, F f) {
      if(l == r) return;
      int l2 = l += b, r2 = r += b;
      prop(l, r);
      for(; l < r; l >>= 1, r >>= 1) {
        if(l & 1) act(l++, f);
        if(r & 1) act(--r, f);
      }
      build(l2, r2);
    }
    void all_apply(F f) { return act(1, f); }
    int max_right(int l, auto g) {
      if(l == n) return n;
      prop(l += b, l + 1);
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
      prop(r += b, r + 1);
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
