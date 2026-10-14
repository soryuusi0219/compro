// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/persistent_vector.hpp
#pragma once
#include <cstring>

namespace soryuusi {

using namespace std;

template<typename T>
struct Persistent_Vector{
  struct node;
  using np=node*;
  np root = nullptr;
  struct node{
    T data;
    np c[20]={};
  };
  Persistent_Vector(){}
  np get_root(){return root;}
  void des_set(int id, T val, np& t){
    if(!t) t = new node();
    if(id == 0) t->data = val;
    else des_set(id/20,val,t->c[id%20]);
  }
  np set(int id, T val, np& t){
    np res = new node();
    if(t){
      memcpy(res->c,t->c,sizeof(t->c));
      res->data = t->data;
    }
    if(id == 0) res->data = val;
    else res->c[id%20]=set(id/20,val,res->c[id%20]);
    return res;
  }
  np mutable_get(int id, np t){
    if(id == 0) return t;
    else return mutable_get(id/20,t->c[id%20]);
  }
  T get(int id, np t){
    if(!t) return T{};
    if(id == 0) return t->data;
    else return get(id/20,t->c[id%20]);
  }
};
//using np = Persistent_Vector<long long>::np;

}  // namespace soryuusi
