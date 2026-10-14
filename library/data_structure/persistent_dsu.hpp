// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/persistent_dsu.hpp
#pragma once
#include <algorithm>
#include <cassert>
#include <utility>

#include "library/data_structure/persistent_vector.hpp"

namespace soryuusi {

using namespace std;

struct Persistent_dsu{
  using PV=Persistent_Vector<pair<int, int>>;
  using np = PV::np;
  PV data;
  int n;
  Persistent_dsu(){}
  np init(int m){
    n = m;
    np res = data.get_root();
    for(int i = 0; i < n; i++) data.des_set(i,{-1,1},res);
    return res;
  }
  int leader(int i,np& t){
    assert(t);
    assert(0 <= i && i < n);
    if(data.get(i,t).first < 0) return i;
    else{
      auto [l,sz] = data.get(i,t);
      int res = leader(l,t);
      if(l != res) t = data.set(i,{res,sz},t);
      return res;
    }
  }
  bool same(int i,int j,np& t){
    assert(t);
    assert(0 <= i && i < n);
    assert(0 <= j && j < n);
    return leader(i,t) == leader(j,t);
  }
  pair<int, np> merge(int i,int j,np& t){
    assert(t);
    assert(0 <= i && i < n);
    assert(0 <= j && j < n);
    int x = leader(i,t); 
    int y = leader(j,t);
    if(x == y) return {x,t};
    int szx = data.get(x,t).second;
    int szy = data.get(y,t).second;
    if(szx < szy){
      np res = data.set(y,{-1,szx+szy},t);
      res = data.set(x,{y,szx+szy},res);
      return {x,res};
    }
    else{
      np res = data.set(x,{-1,szx+szy},t);
      res = data.set(y,{x,szx+szy},res);
      return {x,res};
    }
  }
  int size(int i, np& t){
    assert(t);
    assert(0 <= i && i < n);
    int x = leader(i,t);
    return data.get(x,t).second;
  }
};
using Pdsu = Persistent_dsu;
using np = Pdsu::np;

}  // namespace soryuusi
