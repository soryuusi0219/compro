// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/swag_deque.hpp
#pragma once
#include <cassert>
#include <stack>
#include <type_traits>
#include <utility>
#include <vector>

namespace soryuusi {

using namespace std;

template<typename T, auto op, auto e>
struct swagdq{
  static_assert(is_invocable_r_v<T, decltype(op), T, T>, "op must be callable with (T, T) and return a value convertible to T");
  static_assert(is_invocable_r_v<T, decltype(e)>, "e must be callable with no arguments and return a value convertible to T");

  stack<pair<T,T>> prefix;
  stack<pair<T,T>> suffix;
  swagdq(){
    prefix.push({e(),e()});
    suffix.push({e(),e()});
  };
  bool empty() {return !size();}
  int size() {return prefix.size() + suffix.size() - 2;}
  void p_to_s(){
    int n = prefix.size()-1;
    vector<T> v(n);
    for(int i = 0; i < n; i++){
      v[i] = prefix.top().first; prefix.pop();
    }
    for(int i = (n >> 1); i < n; i++) suffix.push({v[i], op(suffix.top().second,v[i])});
    for(int i = (n >> 1) - 1; i >= 0; i--) prefix.push({v[i], op(v[i],prefix.top().second)});
  }
  void s_to_p(){
    int n = suffix.size()-1;
    vector<T> v(n);
    for(int i = 0; i < n; i++){
      v[i] = suffix.top().first; suffix.pop();
    }
    for(int i = (n >> 1); i < n; i++) prefix.push({v[i], op(v[i],prefix.top().second)});
    for(int i = (n >> 1) - 1; i >= 0; i--) suffix.push({v[i], op(suffix.top().second,v[i])});
  }
  void push_front(T x){
    prefix.push({x, op(x,prefix.top().second)});
  }
  void push_back(T x){
    suffix.push({x, op(suffix.top().second,x)});
  }
  void pop_front(){
    assert(!empty());
    if(prefix.size() == 1) s_to_p();
    prefix.pop();
  }
  void pop_back(){
    assert(!empty());
    if(suffix.size() == 1) p_to_s();
    suffix.pop();
  }
  T prod(){
    return op(prefix.top().second, suffix.top().second);
  }
};

}  // namespace soryuusi
