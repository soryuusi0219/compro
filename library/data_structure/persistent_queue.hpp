// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/persistent_queue.hpp
#pragma once
#include <cassert>
#include <vector>

namespace soryuusi {

using namespace std;

template<class T>
struct Persistent_Queue{
  struct node;
  using np=node*;
  struct que;
  using qp=que*;
  qp origin = nullptr;
  np root = nullptr;
  struct node{
    T data;
    np pr;
    vector<np> dbl;
  };
  struct que{
    np front;
    np back;
    int size;
  };
  Persistent_Queue(){}
  np get_root(){
    return root;
  }
  np get_node(np t, int n){
    if(n == 0) return t;
    int k = 31-__builtin_clz(n);
    if((int)(t->dbl.size()) <= k) return root;
    return get_node(t->dbl[k],n-(1<<k));
  }
  int size(qp q){
    return q->size;
  }
  T front(qp q){
    return get_node(q->back,q->size-1)->data;
  }
  bool empty(qp q){
    return !(q->size);
  }
  np push_node(const T& x, np t){
    np ret = new node;
    ret->pr = t;
    int c = 0;
    while(t!=root){
      ret->dbl.push_back(t);
      if(int(t->dbl.size()) <= c) break;
      t = t->dbl[c];
      c++;
    }
    ret->dbl.push_back(root);
    ret->data = x;
    return ret;
  }
  qp push(const T& x,qp& t){
    qp ret = new que;
    if(!t) {
      t = new que;
      np next = push_node(x,t->back);
      ret->back = next;
      ret->front = next;
      ret->size = 1;
    }
    else{
      np next = push_node(x,t->back);
      ret->back = next;
      ret->front = t->front;
      ret->size = (t->size)+1;
    }
    return ret;
  }
  qp pop(qp& t){
    qp ret = new que;
    assert(!empty(t));
    ret->back = t->back;
    if(t->size == 1) ret->front = t->back;
    else ret->front = get_node(t->back,t->size-2);
    ret->size = (t->size)-1;
    return ret;
  }
};

}  // namespace soryuusi
