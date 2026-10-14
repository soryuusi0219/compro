// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/persistent_dynamic_lazy_segtree.hpp
#pragma once
#include <algorithm>
#include <functional>

namespace soryuusi {

using namespace std;

template<class S, S (*op)(S,S), S (*e)(), class F, F (*mapping)(F,S), F (*composition)(F,F), F (*id)()>
struct Persistent_Dynamic_LazySegtree{
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
    function<S(int, int)> default_prod;
    void push(node* p, int l, int r){
      if(p && p->lz != id()){
        int m = (l+r) >> 1;
        if(l < m) {
          p->lp = p->lp ? new node(*(p->lp)) : new node(default_prod(l,m));
          p->lp->act(p->lz);
        }
        if(m < r){
          p->rp = p->rp ? new node(*(p->rp)) : new node(default_prod(m,r));
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
      p = p ? new node(*p) : new node(default_prod(l,r));
      if(r - l == 1) {
        p->val = x;
        return p;
      }
      push(p,l,r);
      int m = (l+r) >> 1;
      if(i < m) p->lp = _set(p->lp, l, m, i, x);
      else p->rp = _set(p->rp, m, r, i, x);
      calc(p,l,r);
      return p;
    }
    S _get(node*& p, int l, int r, int i){
      // outd(p->val, p->lz, l,r,i);
      if(!p) return default_prod(i,i+1);
      if(r - l == 1) return p->val;
      p = new node(*p);
      push(p,l,r);
      int m = (l+r) >> 1;
      if(i < m) return _get(p->lp, l, m, i);
      else return _get(p->rp, m, r, i);
    }
    S _prod(node*& p, int l, int r, int i, int j){
      if(r <= i || j <= l) return e();
      if(i <= l && r <= j) return p ? p->val : default_prod(l,r);
      p = p ? new node(*p) : new node(default_prod(l,r));
      push(p,l,r);
      int m = (l+r) >> 1;
      return op(_prod(p->lp, l, m, i, j), _prod(p->rp, m, r, i, j));
    }
    node* _apply(node* p, int l, int r, int i, int j, F f){
      // outd(p->val, p->lz, l,r,i,j,f);
      if(r <= i || j <= l) return p;
      p = p ? new node(*p) : new node(default_prod(l,r));
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
    node* _copy_with_apply(node* p, node*& q, int l, int r, int i, int j, F f){
      if(r <= i || j <= l) return p;
      if(i <= l && r <= j) {
        p = q ? new node(*q) : new node(default_prod(l,r));
        p->act(f);
        return p;
      }
      p = p ? new node(*p) : new node(default_prod(l,r));
      q = q ? new node(*q) : new node(default_prod(l,r));
      push(p,l,r);
      push(q,l,r);
      int m = (l+r) >> 1;
      p->lp = _copy_with_apply(p->lp, q->lp, l, m, i, j, f);
      p->rp = _copy_with_apply(p->rp, q->rp, m, r, i, j, f);
      calc(p,l,r);
      return p;
    }
  public:
    using NP = node*;
    Persistent_Dynamic_LazySegtree(int n_) : n(n_), default_prod([](int, int) -> S { return e(); }){}
    template<class X> Persistent_Dynamic_LazySegtree(int n_, X default_prod_) : n(n_), default_prod(default_prod_){}
    NP instance(){return nullptr;}
    NP set(NP p, int i, S x){return _set(p,0,n,i,x);}
    NP apply(NP p, int l, int r, F f){return _apply(p,0,n,l,r,f);}
    NP copy(NP& from, NP to, int l, int r){return _copy_with_apply(to,from,0,n,l,r,id());}
    NP copy_with_apply(NP& from, NP to, int l, int r, F f){return _copy_with_apply(to,from,0,n,l,r,f);}
    S get(NP& p, int i){return _get(p,0,n,i);}
    S prod(NP& p, int l, int r){return _prod(p,0,n,l,r);}
    S all_prod(NP p){return (p ? p->val : e());}
};

}  // namespace soryuusi
