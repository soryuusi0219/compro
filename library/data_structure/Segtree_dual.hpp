// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/Segtree_dual.hpp
namespace soryuusi {

using namespace std;

template<class S, class F, S (*mapping)(F,S), F (*composition)(F,F), F (*id)()>
struct Segtree_dual{
  private:
    int n, h, b;
    vector<S> d;
    vector<F> lz;
    void act(int k, F f) {
      if(k < b) lz[k] = composition(f, lz[k]);
      else if(k < b+n) d[k-n] = mapping(f, d[k-n]);
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
    Segtree_dual(int n_) : n(n_), h((unsigned)n), b(1<<h), d(n), lz(b, id()){}
    Segtree_dual(int n_, S x) : n(n_), h((unsigned)n), b(1<<h), d(n,x), lz(b, id()){}
    Segtree_dual(const vector<S>& v) : n(v.size()), h((unsigned)n), b(1<<h), d(v), lz(b, id()){}
    void set(int i, S a) { d[i] = a;}
    S get(int i){return d[i + b];}
    void apply(int l, int r, F f) {
      if(l == r) return;
      int l2 = l += b, r2 = r += b;
      prop(l, r);
      for(; l < r; l >>= 1, r >>= 1) {
        if(l & 1) act(l++, f);
        if(r & 1) act(--r, f);
      }
    }
    void all_apply(F f){act(1, f);}
};

}  // namespace soryuusi