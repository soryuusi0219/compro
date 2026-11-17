// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/dynamic_segtree.hpp
#pragma once
#include <cstddef>
#include <cassert>
#include <utility>
#include <type_traits>

#include "internal/node_pool.hpp"

namespace soryuusi {

using namespace std;

template<class S, auto op, auto e>
struct Dynamic_Segtree{
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
    node* root;
    void _set(node*& p, int l, int r, int i, S x){
      if(!p) {
        p = pool.create(i, x);
        return;
      }
      if(p->pos == i) {
        p->val = x;
        p->calc();
        return;
      }
      int m = (l+r) >> 1;
      if(i < m) {
        if(i > p->pos) swap(i, p->pos), swap(x, p->val);
        _set(p->lp, l, m, i, x);
      }
      else {
        if(i < p->pos) swap(i, p->pos), swap(x, p->val);
        _set(p->rp, m, r, i, x);
      }
      p->calc();
    }
    S _get(node* p, int l, int r, int i){
      if(!p) return e();
      if(p->pos == i) return p->val;
      int m = (l+r) >> 1;
      if(i < m) return _get(p->lp, l, m, i);
      else return _get(p->rp, m, r, i);
    }
    S _prod(node* p, int l, int r, int i, int j){
      if(!p || r <= i || j <= l) return e();
      if(i <= l && r <= j) return p->sum;
      int m = (l+r) >> 1;
      S res = _prod(p->lp, l, m, i, j);
      if(i <= p->pos && p->pos < j) res = op(res,p->val);
      return op(res,_prod(p->rp, m, r, i, j));
    }
    template<class F>
    int _max_right(node* p, int l, int r, int i, S& x, F& f){
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
    int _min_left(node* p, int l, int r, int i, S& x, F& f){
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
    Dynamic_Segtree(const Dynamic_Segtree&) = delete;
    Dynamic_Segtree& operator=(const Dynamic_Segtree&) = delete;
    Dynamic_Segtree(Dynamic_Segtree&& other) : Dynamic_Segtree(0) {*this = std::move(other);}
    Dynamic_Segtree& operator=(Dynamic_Segtree&& other) {
      if(this == &other) return *this;
      n = std::move(other.n);
      pool = std::move(other.pool);
      root = std::exchange(other.root, nullptr);
      return *this;
    }
    [[nodiscard]] Dynamic_Segtree clone() const {
      Dynamic_Segtree result(n);
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
    Dynamic_Segtree(int n_) : n(n_), root(nullptr){assert(n >= 0);}
    void set(int i, S x){assert(0 <= i && i < n); _set(root,0,n,i,x);}
    S get(int i){assert(0 <= i && i < n); return _get(root,0,n,i);}
    S prod(int l, int r){assert(0 <= l && l <= r && r <= n); return _prod(root,0,n,l,r);}
    S all_prod(){return (root ? root->sum : e());}
    template<class F> int max_right(int i, F f){assert(0 <= i && i <= n && f(e())); S x = e(); return _max_right(root, 0, n, i, x, f);}
    template<class F> int min_left(int i, F f){assert(0 <= i && i <= n && f(e())); S x = e(); return _min_left(root, 0, n, i, x, f);}
};

}  // namespace soryuusi
