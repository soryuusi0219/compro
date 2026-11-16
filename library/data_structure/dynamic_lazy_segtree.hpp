// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/dynamic_lazy_segtree.hpp
#pragma once
#include <cstddef>
#include <cassert>
#include <utility>
#include <functional>
#include <algorithm>
#include <type_traits>

#include "library/data_structure/internal/node_pool.hpp"

namespace soryuusi {

using namespace std;

template<class S, auto op, auto e, class F, auto mapping, auto composition, auto id>
struct Dynamic_LazySegtree{
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
    node* root;
    function<S(int, int)> default_prod;
    void push(node* p, int l, int r){
      if(p && p->lz != id()){
        int m = (l+r) >> 1;
        if(l < m) {
          if(!p->lp) p->lp = pool.create(default_prod(l,m));
          p->lp->act(p->lz);
        }
        if(m < r){
          if(!p->rp) p->rp = pool.create(default_prod(m,r));
          p->rp->act(p->lz);
        }
        p->lz = id();
      }
    }
    void calc(node*p, int l, int r){
      if(p){ 
        int m = (l+r) >> 1;
        p->val = op(p->lp ? p->lp->val : default_prod(l,m), p->rp ? p->rp->val : default_prod(m,r));
      }
    }
    void _set(node*& p, int l, int r, int i, S x){
      if(!p) p = pool.create(default_prod(l,r));
      if(r - l == 1) {
        p->val = x;
        p->lz = id();
        return;
      }
      push(p,l,r);
      int m = (l+r) >> 1;
      if(i < m) _set(p->lp, l, m, i, x);
      else _set(p->rp, m, r, i, x);
      calc(p,l,r);
    }
    S _prod(node* p, int l, int r, int i, int j, F acc) const {
      if(r <= i || j <= l) return e();
      if(!p) return mapping(acc, default_prod(std::max(l,i),std::min(r,j)));
      if(i <= l && r <= j) return mapping(acc,p->val);
      acc = composition(acc,p->lz);
      int m = (l+r) >> 1;
      return op(_prod(p->lp,l,m,i,j,acc),_prod(p->rp,m,r,i,j,acc));
    }
    void _apply(node*& p, int l, int r, int i, int j, F f){
      if(r <= i || j <= l) return;
      if(!p) p = pool.create(default_prod(l,r));
      if(i <= l && r <= j) {
        p->act(f);
        return;
      }
      push(p,l,r);
      int m = (l+r) >> 1;
      _apply(p->lp, l, m, i, j, f);
      _apply(p->rp, m, r, i, j, f);
      calc(p,l,r);
    }
  public:
    Dynamic_LazySegtree(const Dynamic_LazySegtree&) = delete;
    Dynamic_LazySegtree& operator=(const Dynamic_LazySegtree&) = delete;
    Dynamic_LazySegtree(Dynamic_LazySegtree&& other) : Dynamic_LazySegtree(0) {*this = std::move(other);}
    Dynamic_LazySegtree& operator=(Dynamic_LazySegtree&& other) {
      if(this == &other) return *this;
      n = std::move(other.n);
      default_prod = other.default_prod;
      pool = std::move(other.pool);
      root = std::exchange(other.root, nullptr);
      return *this;
    }
    [[nodiscard]] Dynamic_LazySegtree clone() const {
      Dynamic_LazySegtree result(n,default_prod);
      auto dfs = [&](auto&& self, node* p) -> node* {
        if(!p) return nullptr;
        node* q = result.pool.create(*p);
        q->lp = self(self,p->lp); q->rp = self(self,p->rp);
        return q;
      };
      result.root = dfs(dfs,root);
      return result;
    }
    std::size_t allocated_nodes() const {return pool.size();}
    Dynamic_LazySegtree(int n_) : n(n_), root(nullptr), default_prod([](int, int) -> S { return e(); }){assert(n >= 0);}
    template<class X> Dynamic_LazySegtree(int n_, X default_prod_) : n(n_), root(nullptr), default_prod(default_prod_){assert(n >= 0);}
    void set(int i, S x){assert(0 <= i && i < n); _set(root,0,n,i,x);}
    S get(int i) const {assert(0 <= i && i < n); return _prod(root,0,n,i,i+1,id());}
    S get(int i, S) const {return get(i);}
    S prod(int l, int r) const {assert(0 <= l && l <= r && r <= n); return _prod(root,0,n,l,r,id());}
    S all_prod(){return (root ? root->val : default_prod(0,n));}
    void apply(int l, int r, F f){assert(0 <= l && l <= r && r <= n); return _apply(root,0,n,l,r,f);}
};

}  // namespace soryuusi
