// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/persistent_dsu_with_data.hpp
#pragma once
#include <algorithm>
#include <cassert>
#include <cstring>
#include <tuple>
#include <type_traits>
#include <utility>

namespace soryuusi {

using namespace std;

template<class S, auto op>
struct Persistent_dsu{
  static_assert(is_invocable_r_v<S, decltype(op), S, S>, "op must be callable with (S, S) and return a value convertible to S");

  struct Persistent_Vector{
    struct node;
    using np=node*;
    np root = nullptr;
    struct node{
      int data;
      S value;
      int size;
      np c[20]={};
    };
    Persistent_Vector(){}
    np get_root(){return root;}
    void des_set(int id, int val, S s, int sz, np& t){
      if(!t) t = new node();
      if(id == 0) {
        t->data = val;
        t->value = s;
        t->size = sz;
      }
      else des_set(id/20,val,s,sz,t->c[id%20]);
    }
    np set(int id, int val, S s, int sz, np& t){
      np res = new node();
      if(t){
        memcpy(res->c,t->c,sizeof(t->c));
        res->data = t->data;
        res->value = t->value;
        res->size = t->size;
      }
      if(id == 0) {
        res->data = val;
        res->value = s;
        res->size = sz;
      }
      else res->c[id%20]=set(id/20,val,s,sz,res->c[id%20]);
      return res;
    }
    np mutable_get(int id, np t){
      if(id == 0) return t;
      else return mutable_get(id/20,t->c[id%20]);
    }
    tuple<long long,S,long long> get(int id, np t){
      if(!t) return {0,S(),0};
      if(id == 0) return {t->data,t->value,t->size};
      else return get(id/20,t->c[id%20]);
    }
  };    
  using PV=Persistent_Vector;
  using np = PV::np;
  PV data;
  int n;
  Persistent_dsu(){}
  np init(int m, S s){
    n = m;
    np res = data.get_root();
    for(int i = 0; i < n; i++) data.des_set(i,-1,s,1,res);
    return res;
  }
  int leader(int i,np& t){
    assert(t);
    assert(0 <= i && i < n);
    if(std::get<0>(data.get(i,t)) < 0) return i;
    else{
      auto [l,s,sz] = data.get(i,t);
      int res = leader(l,t);
      if(l != res) t = data.set(i,res,s,sz,t);
      return res;
    }
  }
  bool same(int i,int j,np& t){
    assert(t);
    assert(0 <= i && i < n);
    assert(0 <= j && j < n);
    return leader(i,t) == leader(j,t);
  }
  pair<long long,np> merge(int i,int j,np& t){
    assert(t);
    assert(0 <= i && i < n);
    assert(0 <= j && j < n);
    int x = leader(i,t); 
    int y = leader(j,t);
    if(x == y) return {x,t};
    auto [_, sx, szx] = data.get(x,t);
    auto [__, sy, szy] = data.get(y,t);
    S sxy = op(sx,sy);
    if(szx < szy){
      np res = data.set(y,-1,sxy,szx+szy,t);
      res = data.set(x,y,sxy,szx+szy,res);
      return {x,res};
    }
    else{
      np res = data.set(x,-1,sxy,szx+szy,t);
      res = data.set(y,x,sxy,szx+szy,res);
      return {x,res};
    }
  }
  S get(int i, np& t){
    assert(t);
    assert(0 <= i && i < n);
    int x = leader(i,t);
    return std::get<1>(data.get(x,t));
  }
  int size(int i, np& t){
    assert(t);
    assert(0 <= i && i < n);
    int x = leader(i,t);
    return std::get<2>(data.get(x,t));
  }
  np set(int i, S s, np& t){
    assert(t);
    assert(0 <= i && i < n);
    int x = leader(i,t);
    auto [l,_,sz] = data.get(x,t);
    return data.set(x,l,s,sz,t);
  }
};

// using S = long long;
// S op(S x, S y) {return x + y;}
// using Pdsu = Persistent_dsu<S,op>;
// using np = Pdsu::np;

}  // namespace soryuusi
