// https://github.com/soryuusi0219/compro/blob/main/library/math/general_floor_sum.hpp
#pragma once
#include <cmath>
#include <type_traits>

namespace soryuusi {

using namespace std;


// culculate y^f(0)xy^(f(1)-f(0))x...x(y^f(n)-f(n-1)) where f(i) = floor((a*i+b)/m)
template<class S, auto op, auto e, auto pow>
S general_floor_sum(long long n, long long m, long long a, long long b, S x, S y){
  static_assert(is_invocable_r_v<S, decltype(op), S, S>, "op must be callable with (S, S) and return a value convertible to S");
  static_assert(is_invocable_r_v<S, decltype(e)>, "e must be callable with no arguments and return a value convertible to S");
  static_assert(is_invocable_r_v<S, decltype(pow), S, long long>, "pow must be callable with (S, long long) and return a value convertible to S");

  x = op(x,pow(y,a/m));
  a %= m;
  const S z = pow(y,b/m);
  b %= m;
  const long long y_max = a*n+b;
  if(y_max < m) return op(z, pow(x,n));
  return op(op(z, general_floor_sum<S,op,e,pow>(y_max/m-1,a,m,m+a-b-1,y,x)), op(y,pow(x,(y_max%m)/a)));
}

}  // namespace soryuusi
