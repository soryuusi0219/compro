// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/persistent_dynamic_lazy_segtree.hpp
#pragma once
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <functional>
#include <optional>
#include <type_traits>
#include <utility>

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
    static constexpr bool comparable_tag = requires(const F& f){static_cast<bool>(f != id());};
    struct Empty {};
    struct Pending {bool active = false;};

    struct node{
      S val;
      F lz = id();
      node* lp;
      node* rp;
      [[no_unique_address]] conditional_t<comparable_tag,Empty,Pending> pending;
      node(const S& value, node* left = nullptr, node* right = nullptr) : val(value), lp(left), rp(right){}
      bool has_action() const {
        if constexpr(comparable_tag) return lz != id();
        else return pending.active;
      }
      void act(const F& f){
        val = mapping(f,val);
        lz = composition(f,lz);
        if constexpr(!comparable_tag) pending.active = true;
      }
    };
    int n;
    internal::NodePool<node> pool;
    function<S(int, int)> default_prod;

    static const F* action(const F& f){
      if constexpr(comparable_tag) if(!(f != id())) return nullptr;
      return &f;
    }
    // The inherited action happened after the node's pending action.
    static const F* descend(const node* p, const F* outer, optional<F>& combined){
      if(!p || !p->has_action()) return outer;
      if(!outer) return &p->lz;
      F next = composition(*outer,p->lz);
      return action(combined.emplace(std::move_if_noexcept(next)));
    }
    S value(const node* p, int l, int r) const {return p ? p->val : default_prod(l,r);}
    node* materialize(node* p, int l, int r, const F* outer){
      if(!outer) return p;
      auto* q = p ? pool.create(*p) : pool.create(default_prod(l,r));
      q->act(*outer);
      return q;
    }
    node* join(node* left, node* right, int l, int m, int r){
      return pool.create(op(value(left,l,m),value(right,m,r)),left,right);
    }
    node* _set(const node* p, int l, int r, int i, const S& x, const F* outer){
      if(r-l == 1) return pool.create(x);
      optional<F> combined;
      outer = descend(p,outer,combined);
      int m = l+(r-l)/2;
      node* left = p ? p->lp : nullptr;
      node* right = p ? p->rp : nullptr;
      if(i < m) {
        left = _set(left,l,m,i,x,outer);
        right = materialize(right,m,r,outer);
      } else {
        left = materialize(left,l,m,outer);
        right = _set(right,m,r,i,x,outer);
      }
      return join(left,right,l,m,r);
    }
    S _prod(const node* p, int l, int r, int i, int j, F acc) const {
      if(!p || (i <= l && r <= j)) return mapping(acc,value(p,std::max(l,i),std::min(r,j)));
      acc = composition(acc,p->lz);
      int m = l+(r-l)/2;
      if(j <= m) return _prod(p->lp,l,m,i,j,acc);
      if(m <= i) return _prod(p->rp,m,r,i,j,acc);
      return op(_prod(p->lp,l,m,i,j,acc),_prod(p->rp,m,r,i,j,acc));
    }
    node* _apply(node* p, int l, int r, int i, int j, const F& f, const F* outer){
      if(r <= i || j <= l) return materialize(p,l,r,outer);
      if(i <= l && r <= j) {
        if(!outer) return materialize(p,l,r,&f);
        const F next = composition(f,*outer);
        return materialize(p,l,r,action(next));
      }
      optional<F> combined;
      outer = descend(p,outer,combined);
      int m = l+(r-l)/2;
      auto* left = _apply(p ? p->lp : nullptr,l,m,i,j,f,outer);
      auto* right = _apply(p ? p->rp : nullptr,m,r,i,j,f,outer);
      return join(left,right,l,m,r);
    }
    node* _copy_with_apply(node* p, node* q, int l, int r, int i, int j, const F* target, const F* source){
      if(r <= i || j <= l) return materialize(p,l,r,target);
      if(i <= l && r <= j) return materialize(q,l,r,source);
      optional<F> target_combined, source_combined;
      target = descend(p,target,target_combined);
      source = descend(q,source,source_combined);
      int m = l+(r-l)/2;
      auto* left = _copy_with_apply(p ? p->lp : nullptr,q ? q->lp : nullptr,l,m,i,j,target,source);
      auto* right = _copy_with_apply(p ? p->rp : nullptr,q ? q->rp : nullptr,m,r,i,j,target,source);
      return join(left,right,l,m,r);
    }
  public:
    using Root = typename internal::NodePool<node>::Root;

    Persistent_Dynamic_LazySegtree(int n_) : n(n_), default_prod([](int, int) -> S {return e();}){assert(n >= 0);}
    template<class X> Persistent_Dynamic_LazySegtree(int n_, X init) : n(n_), default_prod(init){assert(n >= 0);}
    Root empty_root() const {return pool.root(nullptr);}
    Root instance() const {return empty_root();}
    std::size_t allocated_nodes() const {return pool.size();}
    [[nodiscard]] Root set(Root p, int i, S x){assert(0 <= i && i < n); return pool.root(_set(pool.get(p),0,n,i,x,nullptr));}
    [[nodiscard]] Root apply(Root p, int l, int r, F f){
      assert(0 <= l && l <= r && r <= n);
      auto* q = pool.get(p);
      return l == r || !action(f) ? p : pool.root(_apply(q,0,n,l,r,f,nullptr));
    }
    [[nodiscard]] Root copy_range(Root to, Root from, int l, int r){return copy_impl(to,from,l,r,nullptr);}
    [[nodiscard]] Root copy_range_with_apply(Root to, Root from, int l, int r, F f){
      return copy_impl(to,from,l,r,action(f));
    }
    // Compatibility wrappers retain the old source-first order.
    [[nodiscard]] Root copy(Root from, Root to, int l, int r){return copy_range(to,from,l,r);}
    [[nodiscard]] Root copy_with_apply(Root from, Root to, int l, int r, F f){return copy_range_with_apply(to,from,l,r,f);}
    S get(Root p, int i) const {assert(0 <= i && i < n); return prod(p,i,i+1);}
    S prod(Root p, int l, int r) const {assert(0 <= l && l <= r && r <= n); auto q = pool.get(p); return l == r ? e() : _prod(q,0,n,l,r,id());}
    S all_prod(Root p) const {return value(pool.get(p),0,n);}

  private:
    [[nodiscard]] Root copy_impl(Root to, Root from, int l, int r, const F* f){
      assert(0 <= l && l <= r && r <= n);
      auto* p = pool.get(to);
      auto* q = pool.get(from);
      if(l == r || (p == q && !f)) return to;
      return pool.root(_copy_with_apply(p,q,0,n,l,r,nullptr,f));
    }

};

}  // namespace soryuusi
