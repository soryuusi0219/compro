// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/weighted_dsu.hpp
#pragma once
#include <algorithm>
#include <cassert>
#include <utility>
#include <vector>

namespace soryuusi {

using namespace std;

template<typename T, T op(T,T), T e(), T inv(T)>
struct Weighted_dsu{
  int n;
  vector<int> par;
  vector<T> w;
  Weighted_dsu(int m) : n(m), par(n,-1), w(n,e()){}
  int leader(int i){
    assert(0 <= i && i < n);
    if(par[i] < 0) return i;
    int j = par[i];
    par[i] = leader(j);
    w[i] = op(w[i],w[j]);
    return par[i];
  }
  bool same(int i, int j){
    assert(0 <= i && i < n);
    assert(0 <= j && j < n);
    return leader(i) == leader(j);
  }
  T weight(int i, int j){
    assert(0 <= i && i < n);
    assert(0 <= j && j < n);
    assert(same(i,j));
    return op(w[j],inv(w[i]));
  }
  int merge(int i, int j, T a){
    assert(0 <= i && i < n);
    assert(0 <= j && j < n);
    int x = leader(i);
    int y = leader(j);
    if(x == y){
      if(weight(i,j) == a) return x;
      return -1;
    }
    if(par[x] > par[y]){
      swap(i,j);
      swap(x,y);
      a = inv(a);
    }
    par[x] += par[y];
    par[y] = x;
    w[y] = op(op(inv(w[j]),a),w[i]);
    return x;
  }
  int size(int i){
    assert(0 <= i && i < n);
    return -par[leader(i)];
  }
};

// ll op(ll x,ll y) {return x+y;}
// ll e() {return 0;}
// ll inv(ll x) {return -x;}
// Weighted_dsu<ll,op,e,inv>

// bool op(bool x,bool y) {return x != y;}
// bool e() {return false;}
// bool inv(bool x) {return x;}
// Weighted_dsu<bool,op,e,inv>

}  // namespace soryuusi
