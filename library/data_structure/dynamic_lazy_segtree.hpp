// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/dynamic_lazy_segtree.hpp
#pragma once
#include <cstddef>
#include <cassert>
#include <utility>
#include <functional>
#include <algorithm>
#include <type_traits>

#include "internal/node_pool.hpp"

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
    static constexpr bool comparable_tag = requires(const F& f) { static_cast<bool>(f != id()); };
    struct Empty {};
    struct Pending { bool value = false; };
    struct node{
      S val;
      F lz;
      node* lp;
      node* rp;
      [[no_unique_address]] conditional_t<comparable_tag,Empty,Pending> pending;
      node(S val_) : val(val_), lz(id()), lp(nullptr), rp(nullptr){}
      void act(F f){
        val = mapping(f, val);
        lz = composition(f,lz);
        if constexpr(!comparable_tag) pending.value = true;
      }
      bool has_action() const {
        if constexpr(comparable_tag) return static_cast<bool>(lz != id());
        else return pending.value;
      }
      void clear_action(){
        lz = id();
        if constexpr(!comparable_tag) pending.value = false;
      }
    };
    int n;
    internal::NodePool<node> pool;
    node* root;
    function<S(int, int)> default_prod;
    void push(node* p, int l, int r){
      if(!p->has_action()) return;
      int m = l+(r-l)/2;
      if(!p->lp) p->lp = pool.create(default_prod(l,m));
      if(!p->rp) p->rp = pool.create(default_prod(m,r));
      p->lp->act(p->lz);
      p->rp->act(p->lz);
      p->clear_action();
    }
    void calc(node* p, int l, int r, int m){
      p->val = op(p->lp ? p->lp->val : default_prod(l,m),p->rp ? p->rp->val : default_prod(m,r));
    }
    void _set(node*& p, int l, int r, int i, const S& x){
      if(r-l == 1){
        if(!p) p = pool.create(x);
        else {
          p->val = x;
          p->clear_action();
        }
        return;
      }
      if(!p) p = pool.create(e());
      else push(p,l,r);
      int m = l+(r-l)/2;
      if(i < m) _set(p->lp,l,m,i,x);
      else _set(p->rp,m,r,i,x);
      calc(p,l,r,m);
    }
    S _prod(const node* p, int l, int r, int i, int j, F acc) const {
      if(!p) return mapping(acc,default_prod(std::max(l,i),std::min(r,j)));
      if(i <= l && r <= j) return mapping(acc,p->val);
      acc = composition(acc,p->lz);
      int m = l+(r-l)/2;
      if(j <= m) return _prod(p->lp,l,m,i,j,acc);
      if(m <= i) return _prod(p->rp,m,r,i,j,acc);
      return op(_prod(p->lp,l,m,i,j,acc),_prod(p->rp,m,r,i,j,acc));
    }
    void _apply(node*& p, int l, int r, int i, int j, const F& f){
      if(i <= l && r <= j){
        if(!p) p = pool.create(default_prod(l,r));
        p->act(f);
        return;
      }
      if(!p) p = pool.create(e());
      else push(p,l,r);
      int m = l+(r-l)/2;
      if(i < m) _apply(p->lp,l,m,i,j,f);
      if(m < j) _apply(p->rp,m,r,i,j,f);
      calc(p,l,r,m);
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
    S prod(int l, int r) const {assert(0 <= l && l <= r && r <= n); return l == r ? e() : _prod(root,0,n,l,r,id());}
    S all_prod(){return (root ? root->val : default_prod(0,n));}
    void apply(int l, int r, F f){assert(0 <= l && l <= r && r <= n); if(l < r) _apply(root,0,n,l,r,f);}
};

}  // namespace soryuusi
