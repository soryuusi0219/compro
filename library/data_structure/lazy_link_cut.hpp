// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/lazy_link_cut.hpp
#pragma once
#include <cassert>
#include <iostream>
#include <type_traits>
#include <utility>
#include <vector>

namespace soryuusi {

using namespace std;

template<class T, auto op, auto e, class F, auto mapping, auto composition, auto id>
struct lazy_linkcuttree {
  static_assert(is_invocable_r_v<T, decltype(op), T, T>, "op must be callable with (T, T) and return a value convertible to T");
  static_assert(is_invocable_r_v<T, decltype(e)>, "e must be callable with no arguments and return a value convertible to T");
  static_assert(is_invocable_r_v<T, decltype(mapping), F, T>, "mapping must be callable with (F, T) and return a value convertible to T");
  static_assert(is_invocable_r_v<F, decltype(composition), F, F>, "composition must be callable with (F, F) and return a value convertible to F");
  static_assert(is_invocable_r_v<F, decltype(id)>, "id must be callable with no arguments and return a value convertible to F");

  struct node{  
    node* pp;
    node* lp;
    node* rp;
    T value;
    T rsum;
    T lsum;
    bool rev = false;
    F lazy;
    int index;
    node(int i,T v){
      pp = NULL;lp = NULL;rp = NULL;
      value = v; rsum = v; lsum = v;
      index = i;
      lazy = id();
    }
    bool is_root(){
      return !pp || (pp->lp != this && pp->rp != this);
    }
    void cul() {
      rsum = value;
      lsum = value;
      if(rev){
        if(rp) {rsum = op(rp->lsum,rsum); lsum = op(lsum,rp->rsum);}
        if(lp) {lsum = op(lp->rsum,lsum); rsum = op(rsum,lp->lsum);}
      }
      else{
        if(rp) {rsum = op(rp->rsum,rsum); lsum = op(lsum,rp->lsum);}
        if(lp) {lsum = op(lp->lsum,lsum); rsum = op(rsum,lp->rsum);}
      }
    }
    void rotr(){
      node* q = pp;
      node* r = q->pp;
      q->lp=rp;
      if(rp){
        rp->pp = q;
      }
      rp = q;
      q->pp = this;
      pp = r;
      if(r){
        if(r->lp == q) r->lp = this;
        if(r->rp == q) r->rp = this;
      }
      if(q) q->cul();
      cul();
      if(r) r->cul();
    }
    void rotl(){
      node* q = pp;
      node* r = q->pp;
      q->rp=lp;
      if(lp){
        lp->pp = q;
      }
      lp = q;
      q->pp = this;
      pp = r;
      if(r){
        if(r->lp == q) r->lp = this;
        if(r->rp == q) r->rp = this;
      }
      if(q) q->cul();
      cul();
      if(r) r->cul();
    }
    //propagate
    void propagate(F x){
      lazy = composition(x, lazy);
      value = mapping(x, value);
      rsum = mapping(x, rsum);
      lsum = mapping(x, lsum);
    }
    //toglle
    void toggle() {
      swap(lp, rp);
      swap(rsum,lsum);
      rev ^= true;
    }
    //push
    void push() {
      if(lazy != id()) {
        if(lp) lp->propagate(lazy);
        if(rp) rp->propagate(lazy);
        lazy = id();
      }
      if(rev) {
        if(lp) lp->toggle();
        if(rp) rp->toggle();
        rev = false;
      }
    }
    void splay(){
      push();
      while(!is_root()){
        node* q = pp;
        if(q->is_root()){
          q->push(), push();
          if(q->lp == this) rotr();
          else rotl();
        }else{
          node* r = q->pp;
          r->push(); q->push(); push();
          if(r->lp == q){
            if(q->lp == this) {q->rotr(); rotr();}
            else {rotl(); rotr();}
          }else{
            if(q->rp == this) {q->rotl(); rotl();}
            else {rotr(); rotl();}
          }
        }
      }
    }
  };
  //expose
  node* _expose(node* x){
    node* rp = NULL;
    for(node* p = x; p; p = p->pp){
      p->splay();
      p->rp = rp;
      rp = p;
      p->cul();
    }
    x->splay();
    return rp;
  }
  //cut
  void _cut(node* c){
    _expose(c);
    node* p = c-> lp;
    c->lp = NULL;
    p->pp = NULL;
    c->cul();
  }
  //link
  void _link(node* p, node* c){
    _expose(c);
    _expose(p);
    c->pp = p;
    p->rp = c;
    p->cul();
  }
  //evert
  void _evert(node *c) {
    _expose(c);
    c->toggle();
    c->push();
  }
  //LCA
  node* _lca(node *u, node *v) {
    _expose(u);
    return _expose(v);
  }
  //value
  void _revalue(node* c, T x){
    c->value = x;
    for(;!c->is_root();c=c->pp) c->cul();
    c->cul();
  }
  //apply  
  void _apply(node *t, const F &x) {
    _expose(t);
    t->propagate(x);
    t->push();
  }
  //get_path
  vector<int> _get_path(node* x, node* y){
    vector<int> ret;
    auto dfs = [&](auto&& self, node *c) -> void {
      if(!c) return;
      c->push();
      self(self,c->rp);
      ret.push_back(c->index);
      self(self,c->lp);
    };
    _evert(y);
    _expose(x);
    dfs(dfs,x);
    return ret;
  }
  vector<node> tree;
  lazy_linkcuttree(int n){
    for(int i = 0; i < n; i++) tree.push_back(node(i,e()));
  }
  lazy_linkcuttree(int n, T init){
    for(int i = 0; i < n; i++) tree.push_back(node(i,init));
  }
  void print(){
    for(int i = 0; i < tree.size(); i++){
      cout << "i:" << i;
      cout << " p:"; if(tree[i].pp) cout << tree[i].pp->index; else cout << " ";
      cout << " r:"; if(tree[i].rp) cout << tree[i].rp->index; else cout << " ";
      cout << " l:"; if(tree[i].lp) cout << tree[i].lp->index; else cout << " ";
      //cout << " value:" << tree[i].value;
      //cout << " rsum:" << tree[i].rsum;
      //cout << " lsum:" << tree[i].lsum;
      cout << " rev:" << tree[i].rev;
      //cout << " lazy:" << tree[i].lazy;
      cout << endl;
    }
  }
  void link(int i, int j){
    assert(i != j && i >= 0 && j >= 0 && i < (int)tree.size() && j < (int)tree.size());
    _evert(&tree[j]);
    _link(&tree[i],&tree[j]);
  }
  void cut(int i, int j){
    assert(i != j && i >= 0 && j >= 0 && i < (int)tree.size() && j < (int)tree.size());
    _evert(&tree[i]);
    _cut(&tree[j]);
  }
  void expose(int i){
    assert(i >= 0 && i < (int)tree.size());
    _expose(&tree[i]);
  }
  void set(int i, T x){
    assert(i >= 0 && i < (int)tree.size());
    _revalue(&tree[i],x);
  }
  T get(int i){
    assert(i >= 0 && i < (int)tree.size());
    _evert(&tree[i]);
    _expose(&tree[i]);
    return tree[i].rsum;
  }
  void apply(int i, F x){
    assert(i >= 0 && i < (int)tree.size());
    _apply(&tree[i],x);
  }
  void apply(int i, int j, F x){
    assert(i >= 0 && j >= 0 && i < (int)tree.size() && j < (int)tree.size());
    _evert(&tree[j]);
    _apply(&tree[i],x);
  }
  void evert(int i){
    assert(i >= 0 && i < (int)tree.size());
    _evert(&tree[i]);
  }
  T prod(int i){
     assert(i >= 0 && i < (int)tree.size());
    _expose(&tree[i]);
    return tree[i].rsum;
  }
  T prod(int i, int j){
    assert(i >= 0 && j >= 0 && i < (int)tree.size() && j < (int)tree.size());
    _evert(&tree[j]);
    _expose(&tree[i]);
    return tree[i].rsum;
  }
  int lca(int i, int j){
    assert(i >= 0 && j >= 0 && i < (int)tree.size() && j < (int)tree.size());
    return _lca(&tree[i],&tree[j])->index;
  }
  vector<int> get_path(int i,int j){
    assert(i >= 0 && j >= 0 && i < (int)tree.size() && j < (int)tree.size());
    return _get_path(&tree[i],&tree[j]);
  }
};

}  // namespace soryuusi
