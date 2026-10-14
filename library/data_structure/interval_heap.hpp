// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/interval_heap.hpp
#pragma once
#include <cassert>
#include <functional>
#include <utility>
#include <vector>

namespace soryuusi {

using namespace std;

template<typename T, typename Compare = less<T>>
struct Interval_Heap{
  vector<T> data;
  Compare comp;
  int n = 0;
  Interval_Heap(){};
  Interval_Heap(const vector<T> &v, Compare comp_ = Compare()) : data(v), comp(comp_){
    n = data.size();
	  make_heap();
  }
  template<class Iter> Interval_Heap(Iter first, Iter last, Compare comp_ = Compare()) : data(first, last), comp(comp_){
    n = data.size();
	  make_heap();
  }
  bool empty(){return !n;}
  int size(){return n;}
  T get_max(){
    assert(n > 0);
    return data[0];
  }
  T get_min(){
    assert (n > 0);
    return (n > 1 ? data[1] : data[0]);
  }
  void make_heap(){
    for(int i = n-1; i >= 0; i--){
      if(i & 1 && comp(data[i-1],data[i])) swap(data[i-1],data[i]);
      int k = down(i);
      up(k, i);
    }
  }
  void pop_min(){
    assert (n > 0);
    if(n < 3) {
      data.pop_back();
      n--;
    }
    else{
      swap(data[1],data.back()); data.pop_back();
      n--;
      int k = down(1); up(k);
    }
  }
  void pop_max(){
    assert (n > 0);
    if(n < 2){
      data.pop_back();
      n--;
    } 
    else{
      swap(data[0],data.back()); data.pop_back();
      n--;
      int k = down(0); up(k);
    }
  }
  void push(T x){
    n++;
    data.push_back(x);
    up(n-1);
  }
  inline int parent(int k){return ((k>>1)-1)& ~1;}
  int down(int k){
    if(k & 1){
      while(2*k + 1 < n){
        int c = 2*k + 3;
        if(n <= c || comp(data[c-2], data[c])) c-=2;
        if(c < n && comp(data[c], data[k])){
          swap(data[c],data[k]);
          k = c;
        }
        else break;
      }
    }
    else{
      while(2*k+2 < n){
        int c = 2*k + 4;
        if(n <= c || comp(data[c], data[c-2])) c-=2;
        if(c < n && comp(data[k], data[c])){
          swap(data[c],data[k]);
          k = c;
        }
        else break;
      }
    }
    return k;
  }
  int up(int k, int root = 1){
    if((k|1) < n && comp(data[k&~1], data[k|1])){
      swap(data[k&~1],data[k|1]);
      k ^= 1;
    }
    int p;
    while(root < k && comp(data[p=parent(k)], data[k])){
      swap(data[p],data[k]);
      k = p;
    }
    while(root < k && comp(data[k], data[p=parent(k)|1])){
      swap(data[p],data[k]);
      k = p;
    }
    return k;
  }
};

}  // namespace soryuusi
