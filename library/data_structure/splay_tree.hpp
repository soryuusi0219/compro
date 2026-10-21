// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/splay_tree.hpp
#pragma once
#include <algorithm>
#include <iostream>
#include <iterator>
#include <memory>
#include <type_traits>
#include <utility>
#include <vector>

namespace soryuusi {

using namespace std;

template<class T, auto op, auto e>
struct splay_tree {
  static_assert(is_invocable_r_v<T, decltype(op), T, T>, "op must be callable with (T, T) and return a value convertible to T");
  static_assert(is_invocable_r_v<T, decltype(e)>, "e must be callable with no arguments and return a value convertible to T");

  struct node{  
    node* pp = nullptr;
    node* lp = nullptr;
    node* rp = nullptr;
    T value = e();
    T rsum = e();
    T lsum = e();
    bool rev = false;
    int index = -1;
    int z = 0;
    int sumz = 0;
    node(int i, T v) : value(v), rsum(v), lsum(v), index(i), z(1), sumz(1){}
    void cul() {
      rsum = value;
      lsum = value;
      sumz = z;
      if(rp) {
        sumz += rp->sumz;
        rsum = op(rp->rsum,rsum);
        lsum = op(lsum,rp->lsum); 
      }
      if(lp) {
        sumz += lp->sumz;
        lsum = op(lp->lsum,lsum);
        rsum = op(rsum,lp->rsum);
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
    //toglle
    void toggle() {
      swap(lp, rp);
      swap(rsum,lsum);
      rev ^= true;
    }
    //push
    void push() {
      if(rev) {
        if(lp) lp->toggle();
        if(rp) rp->toggle();
        rev = false;
      }
    }
    void splay(){
      push();
      while(pp){
        node* q = pp;
        if(!(q->pp)){
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
  using pnode = unique_ptr<node>;
  vector<pnode> nodes;
  node* most_left(node* c){
    while(c -> lp) {
      c->push();
      c = c -> lp;
    }
    c->splay();
    return c;
  }
  node* most_right(node* c){
    while(c -> rp){
      c->push();
      c = c -> rp;
    }
    c->splay();
    return c;
  }
  node* kth_node(node* c, int k){
    if(!c) return nullptr;
    while(true){
      c->push();
      int lsumz = 0;
      if(c->lp) lsumz = c->lp->sumz;
      if(lsumz == k) break;
      if(lsumz > k){
        c = c->lp;
        continue;
      }
      if(!c->rp) break;
      k--;
      if(c->lp) k -= c->lp->sumz;
      c = c->rp;
    }
    c->splay();
    return c;
  }
  node* next(node* c){
    if(!c) return {nullptr};
    c->splay();
    if(!c -> rp) return nullptr;
    c->push();
    node* d = most_left(c->rp);
    d->splay();
    return d;
  }
  node* prev(node* c){
    if(!c) return {nullptr};
    c->splay();
    if(!c -> lp) return nullptr;
    c->push();
    node* d = most_right(c->lp);
    d->splay();
    return d;
  }
  pair<node*, node*> split_right(node* c){
    if(!c) return {nullptr,nullptr};
    node* d = next(c);
    if(!d) return {c,nullptr};
    d->push();
    c->pp = nullptr;
    d->lp = nullptr;
    d->cul();
    return {c,d};
  }
  pair<node*, node*> split_left(node* c){
    if(!c) return {nullptr,nullptr};
    node* d = prev(c);
    if(!d) return {nullptr,c};
    d->push();
    c->pp = nullptr;
    d->rp = nullptr;
    d->cul();
    return {d,c};
  }
  node* merge(node* c, node* d){
    if(!c) return d;
    if(!d) return c;
    c->splay();
    d->splay();
    d = most_left(d);
    d->splay();
    if(c->pp) return d;
    d-> push();
    c->pp = d;
    d->lp = c;
    d->cul();
    return d;
  }
  node* make_node(T v){
    int index = nodes.size();
    nodes.push_back(make_unique<node>(node(index,v)));
    return nodes.back().get();
  }
  void delete_node(node* c){
    if(!c) return;
    int i = c->index;
    if(i < (int)nodes.size()-1){
      node* d = nodes.back().get();
      swap(c->index,d->index);
      swap(nodes[i],nodes.back());
    }
    nodes.pop_back();
  }
  node* insert_right(node* c, T v){
    node* d = make_node(v);
    auto [c1, c2] = split_right(c);
    return merge(merge(c1,d),c2);
  }
  node* insert_left(node* c, T v){
    node* d = make_node(v);
    auto [c1, c2] = split_left(c);
    return merge(merge(c1,d),c2);
  }
  node* erase(node* c){
    if(!c) return nullptr;
    c->splay();
    c->push();
    node* d1 = c->lp;
    node* d2 = c->rp;
    if(d1) d1->pp = nullptr;
    if(d2) d2->pp = nullptr;
    delete_node(c);
    return merge(d1,d2);
  }

  node* root = nullptr;
  int size(){return nodes.size();}
  void insert_at(int k, T v){
    if(k==0) root = merge(make_node(v),root);
    else root = insert_right(kth_node(root,k-1),v);
  }
  void erase_at(int k){
    root = erase(kth_node(root,k));
  }
  T get(int k){
    root = kth_node(root,k);
    if(!root) return e();
    return root->value;
  }
  void flip(int l, int r){
    if(l >=  r) return;
    auto [c01,c2] = split_right(kth_node(root,r-1));
    auto [c0,c1] = split_left(kth_node(c01,l));
    if(c1) c1->toggle();
    root = merge(merge(c0,c1),c2);
  }
  void flip_all(){
    if(root) root->toggle;
  }
  T prod(int l, int r){
    if(l >=  r) return e();
    auto [c01,c2] = split_right(kth_node(root,r-1));
    auto [c0,c1] = split_left(kth_node(c01,l));
    T ret = (c1 ? c1->lsum : e());
    root = merge(merge(c0,c1),c2);
    return ret;
  }
  T prodrev(int l, int r){
    if(l >=  r) return e();
    auto [c01,c2] = split_right(kth_node(root,r-1));
    auto [c0,c1] = split_left(kth_node(c01,l));
    T ret = (c1 ? c1->rsum : e());
    root = merge(merge(c0,c1),c2);
    return ret;
  }
  T prod_all(){
    if(!root) return e();
    return root->lsum;
  }
  T prodrev_all(){
    if(!root) return e();
    return root->rsum;
  }
  splay_tree(){}
  splay_tree(vector<T>& v){
    int n = v.size();
    for(int i = 0; i < n; i++) make_node(v[i]);
    auto dfs = [&](auto&& self, int l, int r) -> node* {
      if(l >= r) return (node*)nullptr;
      if(l == r-1) return nodes[l].get();
      int mid = (r+l)/2;
      node* c = nodes[mid].get();
      node* d1 = self(self,l,mid);
      node* d2 = self(self,mid+1,r);
      if(d1) {d1->pp = c;c->lp = d1;}
      if(d2) {d2->pp = c;c->rp = d2;}
      c->cul();
      return c;
    };
    root = dfs(dfs,0,n);
  }
  
  void print(){
    for(int i; i < (int)nodes.size(); i++){
      node* c = nodes[i].get();
      cout << "i:" << c->index;
      cout << " p:"; if(c->pp) cout << c->pp->index; else cout << " ";
      cout << " r:"; if(c->rp) cout << c->rp->index; else cout << " ";
      cout << " l:"; if(c->lp) cout << c->lp->index; else cout << " ";
      cout << " sumz:" << c->sumz;
      cout << " value:" << c->value.fi.val();
      cout << " rsum:" << c->rsum.fi.val();
      cout << " lsum:" << c->lsum.fi.val();
      cout << endl;
    }
  }
};

}  // namespace soryuusi
