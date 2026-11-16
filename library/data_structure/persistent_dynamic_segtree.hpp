// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/persistent_dynamic_segtree.hpp
#pragma once
#include <cassert>
#include <cstddef>
#include <type_traits>
#include <utility>

#include "library/data_structure/internal/node_pool.hpp"

namespace soryuusi {

using namespace std;

template<class S, auto op, auto e>
struct PersistentDynamicSegtree{
  static_assert(is_invocable_r_v<S, decltype(op), S, S>, "op must be callable with (S, S) and return a value convertible to S");
  static_assert(is_invocable_r_v<S, decltype(e)>, "e must be callable with no arguments and return a value convertible to S");

  private:
    struct node{
      int pos;
      S val, sum;
      node* lp;
      node* rp;
      node(int pos_, S val_) : pos(pos_), val(val_), sum(val), lp(nullptr), rp(nullptr) {}
      void calc(){sum = op(lp ? lp->sum : e(), op(val, rp ? rp->sum : e()));}
    };
    int n;
    internal::NodePool<node> pool;
    node* _set(node* p, int l, int r, int i, S x){
      if(!p) return pool.create(i, x);
      node* res = pool.create(*p);
      if(res->pos == i) {
        res->val = x;
        res->calc();
        return res;
      }
      int m = (l+r) >> 1;
      if(i < m) {
        if(i > res->pos) swap(i, res->pos), swap(x, res->val);
        res->lp = _set(res->lp, l, m, i, x);
      }
      else {
        if(i < res->pos) swap(i, res->pos), swap(x, res->val);
        res->rp = _set(res->rp, m, r, i, x);
      }
      res->calc();
      return res;
    }
    S _get(node* p, int l, int r, int i) const {
      if(!p) return e();
      if(p->pos == i) return p->val;
      int m = (l+r) >> 1;
      if(i < m) return _get(p->lp, l, m, i);
      else return _get(p->rp, m, r, i);
    }
    S _prod(node* p, int l, int r, int i, int j) const {
      if(!p || r <= i || j <= l) return e();
      if(i <= l && r <= j) return p->sum;
      int m = (l+r) >> 1;
      S res = _prod(p->lp, l, m, i, j);
      if(i <= p->pos && p->pos < j) res = op(res,p->val);
      return op(res,_prod(p->rp, m, r, i, j));
    }
    template<class F>
    int _max_right(node* p, int l, int r, int i, S& x, F& f) const {
      if(r <= i) return i;
      if(!f(x)) return l;
      if(!p) return r;
      if(f(op(x,p->sum))){
        x = op(x,p->sum);
        return r;
      }
      int m = (l+r) >> 1;
      int res = _max_right(p->lp, l, m, i, x, f);
      if(res < m) return res;
      if(i <= p->pos && !f(x = op(x,p->val))) return p->pos;
      return _max_right(p->rp, m, r, res, x, f);
    }
    template<class F>
    int _min_left(node* p, int l, int r, int i, S& x, F& f) const {
      if(i <= l) return i;
      if(!f(x)) return r;
      if(!p) return l;
      if(f(op(p->sum,x))){
        x = op(p->sum,x);
        return l;
      }
      int m = (l+r) >> 1;
      int res = _min_left(p->rp, m, r, i, x, f);
      if(res > m) return res;
      if(i > p->pos && !f(x = op(p->val,x))) return p->pos+1;
      return _min_left(p->lp, l, m, res, x, f);
    }
  public:
    using Root = typename internal::NodePool<node>::Root;

    PersistentDynamicSegtree(int n_) : n(n_){assert(n >= 0);}
    Root empty_root() const {return pool.root(nullptr);}
    Root instance() const {return empty_root();}
    std::size_t allocated_nodes() const {return pool.size();}
    [[nodiscard]] Root set(Root p, int i, S x){assert(0 <= i && i < n); return pool.root(_set(pool.get(p),0,n,i,x));}
    S get(Root p, int i) const {assert(0 <= i && i < n); return _get(pool.get(p),0,n,i);}
    S prod(Root p, int l, int r) const {assert(0 <= l && l <= r && r <= n); return _prod(pool.get(p),0,n,l,r);}
    S all_prod(Root p) const {auto q = pool.get(p); return q ? q->sum : e();}
    template<class F> int max_right(Root p, int i, F f) const {assert(0 <= i && i <= n && f(e())); S x = e(); return _max_right(pool.get(p),0,n,i,x,f);}
    template<class F> int min_left(Root p, int i, F f) const {assert(0 <= i && i <= n && f(e())); S x = e(); return _min_left(pool.get(p),0,n,i,x,f);}

};

}  // namespace soryuusi
