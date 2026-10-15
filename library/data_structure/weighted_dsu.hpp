// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/weighted_dsu.hpp
#pragma once
#include <algorithm>
#include <cassert>
#include <type_traits>
#include <utility>
#include <vector>

namespace soryuusi {

using namespace std;

template<typename T, auto op, auto e, auto inv>
struct Weighted_dsu{
  static_assert(is_invocable_r_v<T, decltype(op), T, T>, "op must be callable with (T, T) and return a value convertible to T");
  static_assert(is_invocable_r_v<T, decltype(e)>, "e must be callable with no arguments and return a value convertible to T");
  static_assert(is_invocable_r_v<T, decltype(inv), T>, "inv must be callable with (T) and return a value convertible to T");

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

namespace internal {

  template<class T>
  struct weighted_dsu_sum {
    static constexpr T (*op)(T,T) = [](T x, T y)->T {return x+y;};
    static constexpr T (*e)() = []()->T {return T(0);};
    static constexpr T (*inv)(T) = [](T x)->T {return -x;};
  };

  template<class T>
  struct weighted_dsu_xor {
    static constexpr T (*op)(T,T) = [](T x, T y)->T {return x^y;};
    static constexpr T (*e)() = []()->T {return T(0);};
    static constexpr T (*inv)(T) = [](T x)->T {return x;};
  };

}  // namespace internal

template<typename T>
using Weighted_dsu_sum = Weighted_dsu<T,internal::weighted_dsu_sum<T>::op,internal::weighted_dsu_sum<T>::e,internal::weighted_dsu_sum<T>::inv>;

template<typename T>
using Weighted_dsu_xor = Weighted_dsu<T,internal::weighted_dsu_xor<T>::op,internal::weighted_dsu_xor<T>::e,internal::weighted_dsu_xor<T>::inv>;

using Weighted_dsu_bool = Weighted_dsu_xor<bool>;

}  // namespace soryuusi
