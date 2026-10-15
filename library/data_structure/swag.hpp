// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/swag.hpp
#pragma once
#include <cassert>
#include <stack>
#include <type_traits>

namespace soryuusi {

using namespace std;

template<typename T, auto op, auto e>
struct swag{
  static_assert(is_invocable_r_v<T, decltype(op), T, T>, "op must be callable with (T, T) and return a value convertible to T");
  static_assert(is_invocable_r_v<T, decltype(e)>, "e must be callable with no arguments and return a value convertible to T");

  stack<T> prefix;
  stack<T> suffix;
  T prefixprod;
  swag() : prefixprod(e()) {
    suffix.push(e());
  };
  inline bool empty() const {return !size();}
  inline int size() const {return prefix.size() + suffix.size() - 1;}
  void p_to_s(){
    while(prefix.size()){
      suffix.push(op(suffix.top(),prefix.top()));
      prefix.pop();
    }
    prefixprod = e();
  }
  void push(T x){
    prefix.push(x);
    prefixprod = op(x,prefixprod);
  }
  void pop(){
    assert(size());
    if(suffix.size() == 1) p_to_s();
    suffix.pop();
  }
  T prod() const {
    return op(prefixprod, suffix.top());
  }
};

}  // namespace soryuusi
