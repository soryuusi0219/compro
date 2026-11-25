// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/persistent_dynamic_segtree.hpp
#pragma once
#include <cassert>
#include <cstddef>
#include <type_traits>
#include <utility>

#include "internal/node_pool.hpp"

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
      void calc(){
        sum = rp ? op(val,rp->sum) : val;
        if(lp) sum = op(lp->sum,sum);
      }
    };
    int n;
    internal::NodePool<node> pool;
    node* _set(const node* p, int l, int r, int i, S x){
      if(!p) return pool.create(i, x);
      node* res = pool.create(*p);
      if(res->pos == i) {
        res->val = x;
        res->calc();
        return res;
      }
      int m = l+(r-l)/2;
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
    S _get(const node* p, int l, int r, int i) const {
      while(p){
        if(p->pos == i) return p->val;
        int m = l+(r-l)/2;
        if(i < m) p = p->lp, r = m;
        else p = p->rp, l = m;
      }
      return e();
    }
    S _prod(const node* p, int l, int r, int i, int j) const {
      if(!p) return e();
      if(i <= l && r <= j) return p->sum;
      int m = l+(r-l)/2;
      S res = i < m ? _prod(p->lp,l,m,i,j) : e();
      if(i <= p->pos && p->pos < j) res = op(res,p->val);
      return m < j && p->rp ? op(res,_prod(p->rp,m,r,i,j)) : res;
    }
    template<class F>
    int _max_right(const node* p, int l, int r, int i, S& x, F& f) const {
      if(r <= i) return i;
      if(!p) return r;
      if(i <= l){
        S candidate = op(x,p->sum);
        if(f(candidate)){
          x = candidate;
          return r;
        }
      }
      int m = l+(r-l)/2;
      int res = _max_right(p->lp, l, m, i, x, f);
      if(res < m) return res;
      if(i <= p->pos && !f(x = op(x,p->val))) return p->pos;
      return _max_right(p->rp, m, r, res, x, f);
    }
    template<class F>
    int _min_left(const node* p, int l, int r, int i, S& x, F& f) const {
      if(i <= l) return i;
      if(!p) return l;
      if(r <= i){
        S candidate = op(p->sum,x);
        if(f(candidate)){
          x = candidate;
          return l;
        }
      }
      int m = l+(r-l)/2;
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
    S prod(Root p, int l, int r) const {assert(0 <= l && l <= r && r <= n); auto q = pool.get(p); return l == r ? e() : _prod(q,0,n,l,r);}
    S all_prod(Root p) const {auto q = pool.get(p); return q ? q->sum : e();}
    template<class F> int max_right(Root p, int i, F f) const {assert(0 <= i && i <= n && f(e())); S x = e(); return _max_right(pool.get(p),0,n,i,x,f);}
    template<class F> int min_left(Root p, int i, F f) const {assert(0 <= i && i <= n && f(e())); S x = e(); return _min_left(pool.get(p),0,n,i,x,f);}

};

}  // namespace soryuusi
