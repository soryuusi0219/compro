// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/dynamic_lazy_segtree.hpp
#pragma once
#include <functional>
#include <type_traits>

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
    node* root;
    function<S(int, int)> default_prod;
    void push(node* p, int l, int r){
      if(p && p->lz != id()){
        int m = (l+r) >> 1;
        if(l < m) {
          if(!p->lp) p->lp = new node(default_prod(l,m));
          p->lp->act(p->lz);
        }
        if(m < r){
          if(!p->rp) p->rp = new node(default_prod(m,r));
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
      if(!p) p = new node(default_prod(l,r));
      if(r - l == 1) {
        p->val = x;
        return;
      }
      push(p,l,r);
      int m = (l+r) >> 1;
      if(i < m) _set(p->lp, l, m, i, x);
      else _set(p->rp, m, r, i, x);
      calc(p,l,r);
    }
    S _get(node* p, int l, int r, int i){
      if(!p) return default_prod(i,i+1);
      if(r - l == 1) return p->val;
      push(p,l,r);
      int m = (l+r) >> 1;
      if(i < m) return _get(p->lp, l, m, i);
      else return _get(p->rp, m, r, i);
    }
    S _prod(node*& p, int l, int r, int i, int j){
      if(r <= i || j <= l) return e();
      if(i <= l && r <= j) return p ? p->val : default_prod(l,r);
      if(!p) p = new node(default_prod(l,r));
      push(p,l,r);
      int m = (l+r) >> 1;
      return op(_prod(p->lp, l, m, i, j), _prod(p->rp, m, r, i, j));
    }
    void _apply(node*& p, int l, int r, int i, int j, F f){
      if(r <= i || j <= l) return;
      if(!p) p = new node(default_prod(l,r));
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
    Dynamic_LazySegtree(int n_) : n(n_), root(nullptr), default_prod([](int, int) -> S { return e(); }){}
    template<class X> Dynamic_LazySegtree(int n_, X default_prod_) : n(n_), root(nullptr), default_prod(default_prod_){}
    void set(int i, S x){_set(root,0,n,i,x);}
    S get(int i, S x){return _get(root,0,n,i);}
    S prod(int l, int r){return _prod(root,0,n,l,r);}
    S all_prod(){return (root ? root->val : default_prod(0,n));}
    void apply(int l, int r, F f){return _apply(root,0,n,l,r,f);}
};

}  // namespace soryuusi
