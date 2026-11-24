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
    const int keep = (prefix.size()-1)/2;
    vector<T> saved;
    saved.reserve(keep);
    for(int i = 0; i < keep; ++i){
      saved.push_back(std::move_if_noexcept(prefix.top().first));
      prefix.pop();
    }
    while(prefix.size() > 1){
      push_back(std::move_if_noexcept(prefix.top().first));
      prefix.pop();
    }
    for(auto it = saved.rbegin(); it != saved.rend(); ++it) push_front(std::move_if_noexcept(*it));
  }
  void s_to_p(){
    const int keep = (suffix.size()-1)/2;
    vector<T> saved;
    saved.reserve(keep);
    for(int i = 0; i < keep; ++i){
      saved.push_back(std::move_if_noexcept(suffix.top().first));
      suffix.pop();
    }
    while(suffix.size() > 1){
      push_front(std::move_if_noexcept(suffix.top().first));
      suffix.pop();
    }
    for(auto it = saved.rbegin(); it != saved.rend(); ++it) push_back(std::move_if_noexcept(*it));
  }
  void push_front(T x){
    T aggregate = prefix.size() == 1 ? x : op(x,prefix.top().second);
    prefix.emplace(std::move_if_noexcept(x),std::move_if_noexcept(aggregate));
  }
  void push_back(T x){
    T aggregate = suffix.size() == 1 ? x : op(suffix.top().second,x);
    suffix.emplace(std::move_if_noexcept(x),std::move_if_noexcept(aggregate));
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
    if(prefix.size() == 1) return suffix.top().second;
    if(suffix.size() == 1) return prefix.top().second;
    return op(prefix.top().second,suffix.top().second);
  }
};

}  // namespace soryuusi
