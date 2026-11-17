// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/persistent_dynamic_lazy_segtree.hpp
#pragma once
#include <cassert>
#include <cstddef>
#include <algorithm>
#include <functional>
#include <type_traits>

#include "internal/node_pool.hpp"

namespace soryuusi {

using namespace std;

template<class S, auto op, auto e, class F, auto mapping, auto composition, auto id>
struct Persistent_Dynamic_LazySegtree{
  static_assert(is_invocable_r_v<S, decltype(op), S, S>, "op must be callable with (S, S) and return a value convertible to S");
  static_assert(is_invocable_r_v<S, decltype(e)>, "e must be callable with no arguments and return a value convertible to S");
  static_assert(is_invocable_r_v<S, decltype(mapping), F, S>, "mapping must be callable with (F, S) and return a value convertible to S");
  static_assert(is_invocable_r_v<F, decltype(composition), F, F>, "composition must be callable with (F, F) and return a value convertible to F");
  static_assert(is_invocable_r_v<F, decltype(id)>, "id must be callable with no arguments and return a value convertible to F");

  private:
    struct node{
      S val;
      F lz;
      node* lp;
      node* rp;
      node(S val_) : val(val_), lz(id()), lp(nullptr), rp(nullptr){}
      void act(F f){
        val = mapping(f, val);
        lz = composition(f,lz);
      }
    };
    int n;
    internal::NodePool<node> pool;
    function<S(int, int)> default_prod;
    void push(node* p, int l, int r){
      if(p && p->lz != id()){
        int m = (l+r) >> 1;
        if(l < m) {
          p->lp = p->lp ? pool.create(*(p->lp)) : pool.create(default_prod(l,m));
          p->lp->act(p->lz);
        }
        if(m < r){
          p->rp = p->rp ? pool.create(*(p->rp)) : pool.create(default_prod(m,r));
          p->rp->act(p->lz);
        }
        p->lz = id();
      }
    }
    void calc(node*p, int l, int r){
      int m = (l+r) >> 1;
      p->val = op(p->lp ? p->lp->val : default_prod(l,m), p->rp ? p->rp->val : default_prod(m,r));
    }
    node* _set(node* p, int l, int r, int i, S x){
      p = p ? pool.create(*p) : pool.create(default_prod(l,r));
      if(r - l == 1) {
        p->val = x;
        p->lz = id();
        return p;
      }
      push(p,l,r);
      int m = (l+r) >> 1;
      if(i < m) p->lp = _set(p->lp, l, m, i, x);
      else p->rp = _set(p->rp, m, r, i, x);
      calc(p,l,r);
      return p;
    }
    S _prod(node* p, int l, int r, int i, int j, F acc) const {
      if(r <= i || j <= l) return e();
      if(!p) return mapping(acc, default_prod(std::max(l,i),std::min(r,j)));
      if(i <= l && r <= j) return mapping(acc,p->val);
      acc = composition(acc,p->lz);
      int m = (l+r) >> 1;
      return op(_prod(p->lp,l,m,i,j,acc),_prod(p->rp,m,r,i,j,acc));
    }
    node* _apply(node* p, int l, int r, int i, int j, F f){
      // outd(p->val, p->lz, l,r,i,j,f);
      if(r <= i || j <= l) return p;
      p = p ? pool.create(*p) : pool.create(default_prod(l,r));
      if(i <= l && r <= j) {
        p->act(f);
        return p;
      }
      push(p,l,r);
      int m = (l+r) >> 1;
      p->lp = _apply(p->lp, l, m, i, j, f);
      p->rp = _apply(p->rp, m, r, i, j, f);
      calc(p,l,r);
      return p;
    }
    node* _copy_with_apply(node* p, node* q, int l, int r, int i, int j, F f){
      if(r <= i || j <= l) return p;
      if(i <= l && r <= j) {
        if(f == id()) return q;
        p = q ? pool.create(*q) : pool.create(default_prod(l,r));
        p->act(f);
        return p;
      }
      p = p ? pool.create(*p) : pool.create(default_prod(l,r));
      push(p,l,r);
      // f includes the source's ancestor tags, from outermost to innermost.
      F next = q ? composition(f,q->lz) : f;
      int m = (l+r) >> 1;
      p->lp = _copy_with_apply(p->lp,q ? q->lp : nullptr,l,m,i,j,next);
      p->rp = _copy_with_apply(p->rp,q ? q->rp : nullptr,m,r,i,j,next);
      calc(p,l,r);
      return p;
    }
  public:
    using Root = typename internal::NodePool<node>::Root;

    Persistent_Dynamic_LazySegtree(int n_) : n(n_), default_prod([](int, int) -> S {return e();}){assert(n >= 0);}
    template<class X> Persistent_Dynamic_LazySegtree(int n_, X init) : n(n_), default_prod(init){assert(n >= 0);}
    Root empty_root() const {return pool.root(nullptr);}
    Root instance() const {return empty_root();}
    std::size_t allocated_nodes() const {return pool.size();}
    [[nodiscard]] Root set(Root p, int i, S x){assert(0 <= i && i < n); return pool.root(_set(pool.get(p),0,n,i,x));}
    [[nodiscard]] Root apply(Root p, int l, int r, F f){assert(0 <= l && l <= r && r <= n); return pool.root(_apply(pool.get(p),0,n,l,r,f));}
    [[nodiscard]] Root copy_range(Root to, Root from, int l, int r){return copy_range_with_apply(to,from,l,r,id());}
    [[nodiscard]] Root copy_range_with_apply(Root to, Root from, int l, int r, F f){
      assert(0 <= l && l <= r && r <= n);
      return pool.root(_copy_with_apply(pool.get(to),pool.get(from),0,n,l,r,f));
    }
    // Compatibility wrappers retain the old source-first order.
    [[nodiscard]] Root copy(Root from, Root to, int l, int r){return copy_range(to,from,l,r);}
    [[nodiscard]] Root copy_with_apply(Root from, Root to, int l, int r, F f){return copy_range_with_apply(to,from,l,r,f);}
    S get(Root p, int i) const {assert(0 <= i && i < n); return prod(p,i,i+1);}
    S prod(Root p, int l, int r) const {assert(0 <= l && l <= r && r <= n); return _prod(pool.get(p),0,n,l,r,id());}
    S all_prod(Root p) const {auto q = pool.get(p); return q ? q->val : default_prod(0,n);}

};

}  // namespace soryuusi
