// https://github.com/soryuusi0219/compro/blob/main/library/math/dirichlet_sum.hpp
#pragma once
#include <algorithm>
#include <bit>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <numeric>
#include <type_traits>
#include <utility>
#include <vector>

#include "modint_traits.hpp"

namespace soryuusi {

using namespace std;

template<class T, int id = -1>
struct Dirichlet_Sum{
  inline static long long N = 0, K = 0, L = 0;
  inline static vector<long long> eval_points;
  inline static vector<long long> primes, divisors;

private:
  static long long isqrt(long long x){
    long long r = sqrt(static_cast<double>(x));
    if((__int128)r*r > x) --r;
    if((__int128)(r+1)*(r+1) <= x) ++r;
    return r;
  }

  static void resize_sieve(long long k){
    if(k == K) return;
    divisors.resize(k);
    if(k < K){
      primes.erase(upper_bound(primes.begin(),primes.end(),k),primes.end());
      return;
    }
    auto extend_prime = [&](long long p){
      for(long long x = max(p*p,(K/p+1)*p); x <= k; x += p){
        if(divisors[x-1]) continue;
        long long y = x/p;
        divisors[x-1] = y == p ? 1 : (y%p == 0 ? divisors[y-1] : y);
      }
    };
    for(long long p : primes){
      if(p > k/p) break;
      extend_prime(p);
    }
    for(long long p = max(2LL,K+1); p <= k; ++p){
      if(divisors[p-1]) continue;
      primes.push_back(p);
      if(p <= k/p) extend_prime(p);
    }
  }

  template<bool Divide>
  static void apply_prime_factors(vector<T>& values, const vector<T>& factor){
    for(long long p : primes){
      const T first = factor[p-1];
      const long long end = K/p;
      auto update = [&](long long j){
        T change = first*values[j-1];
        for(long long q = p; j%p == 0; ){
          j /= p;
          q *= p;
          change += factor[q-1]*values[j-1];
        }
        return change;
      };
      if constexpr (Divide){
        for(long long j = 1; j <= end; ++j) values[j*p-1] -= update(j);
      }else{
        for(long long j = end; j >= 1; --j) values[j*p-1] += update(j);
      }
    }
  }

  template<bool Divide>
  static void multiplicative_product(vector<T>& result, const vector<T>& a, const vector<T>& b){
    result[0] = T(1);
    for(long long p : primes){
      for(long long n = p; ; n *= p){
        T value = a[n-1];
        if constexpr (Divide) value -= b[n-1];
        else value += b[n-1];
        for(long long q = p, r = n/p; r > 1; q *= p, r /= p){
          if constexpr (Divide) value -= b[q-1]*result[r-1];
          else value += a[q-1]*b[r-1];
        }
        result[n-1] = value;
        if(n > K/p) break;
      }
    }
    for(long long n = 2; n <= K; ++n){
      long long q = divisors[n-1];
      if(q > 1) result[n-1] = result[q-1]*result[n/q-1];
    }
  }

  static vector<T> prefix(const vector<T>& a){
    vector<T> result(K+1,T(0));
    for(long long i = 0; i < K; ++i) result[i+1] = result[i]+a[i];
    return result;
  }

  // Number of distinct quotient points strictly above the dense table.
  static long long upper_count(){return K < N ? N/(K+1) : 0;}
  static long long large_rows(long long upper){
    long long cube = cbrt(static_cast<double>(N));
    long long cutoff = max(K,cube*cube/4);
    return cutoff >= N ? 0 : min(upper,N/(cutoff+1));
  }

  // Scatter products from the dense tables onto the quotient intervals above K.
  // In division, the terms containing a coefficient at index 1 are solved separately.
  static void middle_products(vector<T>& result, const vector<T>& a, const vector<T>& b,
                              const vector<T>& pa, const vector<T>& pb, long long limit, long long first){
    for(long long i = first; i*i <= limit; ++i){
      const T x = a[i-1], y = b[i-1];
      const long long numerator = N/i;
      if(i*i > K) result[numerator/i-1] += x*y;
      auto scatter = [&](auto qn){
        using Q = decltype(qn);
        const Q end = min(K,limit/i);
        const Q single = min<long long>(end,isqrt(qn));
        Q j = max(i+1,K/i+1);
        for(; j <= single; ++j) result[qn/j-1] += x*b[j-1] + y*a[j-1];
        // Beyond sqrt(qn), several products can land in the same row.
        while(j <= end){
          const Q q = qn/j, last = min(end,qn/q);
          result[q-1] += x*(pb[last]-pb[j-1]) + y*(pa[last]-pa[j-1]);
          j = last+1;
        }
      };
      if(numerator <= numeric_limits<uint32_t>::max()) scatter(static_cast<uint32_t>(numerator));
      else scatter(numerator);
    }
  }

  static constexpr bool use_raw_products = []{
    if constexpr (internal::is_static_modint_like_v<T>) return 0 < T::mod() && T::mod() <= 2147483647;
    else return false;
  }();

  struct Hyperbola {
    const Dirichlet_Sum &a, &b;
    const vector<T> &pa, &pb;
    long long upper;
    vector<uint64_t> reciprocals{};

    void prepare_reciprocals(){
      if(N < (1LL<<24)) return;
      reciprocals.resize(min(1LL<<14,isqrt(N))+1);
      for(size_t i = 2; i < reciprocals.size(); ++i) reciprocals[i] = numeric_limits<uint64_t>::max()/i+1;
    }

    template<bool Square, bool Reciprocal, class X>
    T dot(long long row, X x, long long first, long long end) const {
      using Acc = conditional_t<use_raw_products,__uint128_t,T>;
      Acc sum{};
      auto add = [&](long long i, const T& sa, const T& sb){
        if constexpr (use_raw_products){
          uint64_t term = uint64_t(a.al[i-1].val())*sb.val();
          if constexpr (!Square) term += uint64_t(b.al[i-1].val())*sa.val();
          sum += term;
        }else{
          sum += a.al[i-1]*sb;
          if constexpr (!Square) sum += b.al[i-1]*sa;
        }
      };
      const long long last_upper = min(end,upper/row);
      long long i = first;
      for(long long j = row*i-1; i <= last_upper; ++i, j += row) add(i,a.au[j],b.au[j]);
      if constexpr (Reciprocal && is_same_v<X,uint32_t>){
        const long long stop = min(end+1,(long long)reciprocals.size());
        for(; i < stop; ++i){
          // i >= 2 here. With x < 2^32, ceil(2^64/i) gives the exact quotient.
          auto q = uint64_t((__uint128_t(x)*reciprocals[i])>>64);
          add(i,pa[q],pb[q]);
        }
      }
      for(; i <= end; ++i){
        auto q = x/static_cast<X>(i);
        add(i,pa[q],pb[q]);
      }
      if constexpr (use_raw_products) return T::raw(static_cast<int>(sum%T::mod()));
      else return sum;
    }

    template<bool Square = false, bool Reciprocal = false>
    T sum(long long row, long long first, long long end) const {
      const long long x = eval_points[row-1];
      if(x <= numeric_limits<uint32_t>::max()) return dot<Square,Reciprocal>(row,static_cast<uint32_t>(x),first,end);
      return dot<Square,Reciprocal>(row,x,first,end);
    }
  };

  template<bool Subtract>
  void combine(const Dirichlet_Sum& other){
    auto apply = [](T& a, const T& b){
      if constexpr (Subtract) a -= b;
      else a += b;
    };
    for(long long i = 0; i < K; ++i) apply(al[i],other.al[i]);
    for(long long i = 0; i < L; ++i) apply(au[i],other.au[i]);
    is_multiplicative = false;
  }
  void scale(const T& x){
    for(T& v : al) v *= x;
    for(T& v : au) v *= x;
    is_multiplicative = false;
  }
  void add_constant(const T& x){
    al[0] += x;
    for(T& v : au) v += x;
    is_multiplicative = false;
  }

  static long long power_at_most(long long base, int exponent, long long limit){
    const __uint128_t cap = (__uint128_t)limit+1;
    __uint128_t a = base, result = 1;
    while(exponent){
      if(exponent&1){
        result *= a;
        if(result >= cap) return 0;
      }
      exponent >>= 1;
      a = min(a*a,cap);
    }
    return static_cast<long long>(result);
  }

public:
  static void init(long long n, long long k){
    assert(n > 0 && k >= 0);
    long long root = isqrt(n);
    k = max(k,root+(root*root != n));
    if(n == N && k == K) return;
    resize_sieve(k);
    N = n, K = k, L = (N-1)/K+1;
    eval_points.resize(L);
    for(long long i = 0; i < L; ++i) eval_points[i] = N/(i+1);
  }

  vector<T> al, au;
  bool is_multiplicative = false;
  Dirichlet_Sum() : al(K), au(L) {}
  Dirichlet_Sum(vector<T> lower, vector<T> upper, bool is_mul = false)
    : al(std::move(lower)), au(std::move(upper)), is_multiplicative(is_mul) {
    al.resize(K), au.resize(L);
  }
  Dirichlet_Sum(vector<T> lower, T (*f)(long long), bool is_mul = false)
    : al(std::move(lower)), au(L), is_multiplicative(is_mul) {
    al.resize(K);
    for(long long i = 0; i < L; ++i) au[i] = f(eval_points[i]);
  }
  Dirichlet_Sum(T (*fl)(long long), T (*fu)(long long), bool is_mul = false)
    : al(K), au(L), is_multiplicative(is_mul) {
    for(long long i = 0; i < K; ++i) al[i] = fl(i+1);
    for(long long i = 0; i < L; ++i) au[i] = fu(eval_points[i]);
  }

  Dirichlet_Sum operator+=(const Dirichlet_Sum& other){combine<false>(other); return *this;}
  Dirichlet_Sum operator+(const Dirichlet_Sum& other) const {
    Dirichlet_Sum ret(*this);
    ret.combine<false>(other);
    return ret;
  }
  Dirichlet_Sum operator-=(const Dirichlet_Sum& other){combine<true>(other); return *this;}
  Dirichlet_Sum operator-(const Dirichlet_Sum& other) const {
    Dirichlet_Sum ret(*this);
    ret.combine<true>(other);
    return ret;
  }
  Dirichlet_Sum operator*=(const T x){scale(x); return *this;}
  Dirichlet_Sum operator*(const T x) const {
    Dirichlet_Sum ret(*this);
    ret.scale(x);
    return ret;
  }

  Dirichlet_Sum operator*(const Dirichlet_Sum& other) const {
    Dirichlet_Sum ret;
    const bool square = this == &other;
    if(is_multiplicative && other.is_multiplicative){
      multiplicative_product<false>(ret.al,al,other.al);
      ret.is_multiplicative = true;
    }else if(is_multiplicative || other.is_multiplicative){
      ret.al = is_multiplicative ? other.al : al;
      apply_prime_factors<false>(ret.al,is_multiplicative ? al : other.al);
    }else if(square){
      for(long long i = 1; i*i <= K; ++i){
        const T x = al[i-1];
        ret.al[i*i-1] += x*x;
        const T twice = x+x;
        for(long long j = i+1, k = i*j; k <= K; ++j, k += i) ret.al[k-1] += twice*al[j-1];
      }
    }else{
      for(long long i = 1; i <= K; ++i){
        const T x = al[i-1];
        for(long long j = 0, k = i-1; k < K; ++j, k += i) ret.al[k] += x*other.al[j];
      }
    }
    const auto pa = prefix(al);
    const auto pb_storage = square ? vector<T>{} : prefix(other.al);
    const auto& pb = square ? pa : pb_storage;
    const long long upper = upper_count(), large = large_rows(upper);
    long long i = 0;
    T sum = T(0);
    for(long long d = L; d > upper; --d){
      for(; i < eval_points[d-1]; ++i) sum += ret.al[i];
      ret.au[d-1] = sum;
    }
    if(large < upper){
      middle_products(ret.au,al,other.al,pa,pb,eval_points[large],1);
      T a_prev = pa[K], b_prev = pb[K];
      for(; i < K; ++i) sum += ret.al[i];
      // The product of a dense index i and the bucket N/d lands in row d/i.
      // Two factors above K cannot contribute, since K >= sqrt(N).
      for(uint32_t d = upper; d > large; --d){
        const T x = au[d-1]-a_prev, y = other.au[d-1]-b_prev;
        a_prev = au[d-1], b_prev = other.au[d-1];
        for(uint32_t i = 1, end = d/(large+1); i <= end; ++i) ret.au[d/i-1] += x*other.al[i-1] + y*al[i-1];
        sum += ret.au[d-1];
        ret.au[d-1] = sum;
      }
    }
    Hyperbola dot{*this,other,pa,pb,upper};
    if constexpr (use_raw_products) dot.prepare_reciprocals();
    for(long long d = 1; d <= large; ++d){
      const long long s = isqrt(eval_points[d-1]);
      T sum = square ? dot.template sum<true,use_raw_products>(d,1,s)
                     : dot.template sum<false,use_raw_products>(d,1,s);
      if(square) sum += sum;
      ret.au[d-1] = sum-pa[s]*pb[s];
    }
    return ret;
  }
  Dirichlet_Sum operator*=(const Dirichlet_Sum& other){*this = *this*other; return *this;}

  Dirichlet_Sum operator/(const Dirichlet_Sum& other) const {
    assert(other.al[0] != 0);
    const T inverse = T(1)/other.al[0];
    Dirichlet_Sum ret;
    if(is_multiplicative && other.is_multiplicative){
      multiplicative_product<true>(ret.al,al,other.al);
      ret.is_multiplicative = true;
    }else if(other.is_multiplicative){
      ret.al = al;
      apply_prime_factors<true>(ret.al,other.al);
    }else{
      for(long long i = 1; i <= K; ++i){
        const T x = (al[i-1]-ret.al[i-1])*inverse;
        ret.al[i-1] = x;
        for(long long j = 1, k = 2*i-1; k < K; ++j, k += i) ret.al[k] += x*other.al[j];
      }
    }
    const auto pa = prefix(other.al), pb = prefix(ret.al);
    const long long upper = upper_count(), large = large_rows(upper);
    for(long long d = L; d > upper; --d) ret.au[d-1] = pb[eval_points[d-1]];
    const T first = ret.al[0];
    if(large < upper){
      middle_products(ret.au,other.al,ret.al,pa,pb,eval_points[large],2);
      T a_prev = pa[K], b_prev = pb[K], h_prev = accumulate(al.begin(),al.end(),T(0));
      // Solve each quotient interval from the coefficients at 1 and smaller intervals.
      for(uint32_t d = upper; d > large; --d){
        const T x = other.au[d-1]-a_prev;
        const T y = (au[d-1]-h_prev-first*x-ret.au[d-1])*inverse;
        a_prev = other.au[d-1], h_prev = au[d-1];
        for(uint32_t i = 2, end = d/(large+1); i <= end; ++i) ret.au[d/i-1] += x*ret.al[i-1] + y*other.al[i-1];
        b_prev += y;
        ret.au[d-1] = b_prev;
      }
    }
    const Hyperbola dot{other,ret,pa,pb,upper};
    for(long long d = large; d >= 1; --d){
      const long long s = isqrt(eval_points[d-1]);
      ret.au[d-1] = (au[d-1] - first*other.au[d-1] - dot.sum(d,2,s) + pa[s]*pb[s])*inverse;
    }
    return ret;
  }
  Dirichlet_Sum operator/=(const Dirichlet_Sum& other){*this = *this/other; return *this;}

  Dirichlet_Sum pow(long long k){
    assert(k >= 0);
    if(k == 0) return e();
    const uint64_t exponent = k;
    const int degree = bit_width(static_cast<uint64_t>(max(N,K)))-1;
    if constexpr (requires {al[0] == T(0);}){
      if(k > degree && al[0] == T(0)) return Dirichlet_Sum();
    }
    const int bits = bit_width(exponent);
    if(bits+popcount(exponent)-2 <= degree){
      Dirichlet_Sum ret(*this);
      for(int i = bits-2; i >= 0; --i){
        ret = ret*ret;
        if(exponent>>i&1) ret = ret*(*this);
      }
      return ret;
    }
    // The part with zero coefficient at 1 is nilpotent above degree.
    const T constant = al[0];
    vector<T> coefficients(degree+1,T(0)), next(degree+1,T(0));
    coefficients[0] = T(1);
    for(int i = bits-1; i >= 0; --i){
      fill(next.begin(),next.end(),T(0));
      for(int j = 0; j <= degree; ++j) for(int l = 0; j+l <= degree; ++l) next[j+l] += coefficients[j]*coefficients[l];
      if(exponent>>i&1){
        for(int j = degree; j > 0; --j) next[j] = next[j]*constant+next[j-1];
        next[0] *= constant;
      }
      coefficients.swap(next);
    }
    int last = degree;
    if constexpr (requires {coefficients[0] == T(0);}){
      while(last > 0 && coefficients[last] == T(0)) --last;
    }
    if(last == 0) return e()*coefficients[0];
    Dirichlet_Sum nilpotent(*this);
    nilpotent.add_constant(T(0)-constant);
    Dirichlet_Sum ret = nilpotent*coefficients[last];
    ret.add_constant(coefficients[last-1]);
    for(int j = last-2; j >= 0; --j){
      ret = ret*nilpotent;
      ret.add_constant(coefficients[j]);
    }
    ret.is_multiplicative = is_multiplicative;
    return ret;
  }

  static Dirichlet_Sum e(){
    Dirichlet_Sum result;
    result.al[0] = T(1);
    fill(result.au.begin(),result.au.end(),T(1));
    result.is_multiplicative = true;
    return result;
  }
  static Dirichlet_Sum zeta(){
    return Dirichlet_Sum([](long long) -> T{return 1;}, [](long long x) -> T{return x;}, true);
  }
  static Dirichlet_Sum zetak(int k){
    assert(k >= 1);
    if(k == 1) return zeta();
    if(k >= 63 || max(N,K) < (1LL<<k)) return e();
    auto root = [&](long long x){
      if(k == 2) return isqrt(x);
      long long r = std::pow(static_cast<double>(x),1.0/k);
      while(!power_at_most(r,k,x)) --r;
      while(power_at_most(r+1,k,x)) ++r;
      return r;
    };
    Dirichlet_Sum result;
    for(long long i = 1, r = root(K); i <= r; ++i) result.al[power_at_most(i,k,K)-1] = T(1);
    for(long long i = 0; i < L; ++i) result.au[i] = T(root(eval_points[i]));
    result.is_multiplicative = true;
    return result;
  }
  static Dirichlet_Sum zetashift(int k){
    assert(1 <= k && k <= 5);
    if(k == 1) return Dirichlet_Sum([](long long x) -> T{return x;}, [](long long x) -> T{T tx = x; return tx*(tx+1)/2;}, true);
    else if(k == 2) return Dirichlet_Sum([](long long x) -> T{T tx = x; return tx*tx;}, [](long long x) -> T{T tx = x; return tx*(tx+1)*(tx*2+1)/6;}, true);
    else if(k == 3) return Dirichlet_Sum([](long long x) -> T{T tx = x; return tx*tx*tx;}, [](long long x) -> T{T tx = x; return tx*(tx+1)*(tx+1)/4;}, true);
    else if(k == 4) return Dirichlet_Sum([](long long x) -> T{T tx = x; return tx*tx*tx*tx;}, [](long long x) -> T{T tx = x; return tx*(tx+1)*(tx*2+1)*(3*tx*(tx+1)+1)/30;}, true);
    else return Dirichlet_Sum([](long long x) -> T{T tx = x; return tx*tx*tx*tx*tx;}, [](long long x) -> T{T tx = x; return tx*tx*(tx+1)*(tx+1)*(2*tx*tx+2*tx-1)/12;}, true);
  }
  T getN(){return au[0];}
  T getN() const {return au[0];}
  T getNdivi(int i){return au[i-1];}
  T getNdivi(int i) const {return au[i-1];}
  vector<T> getupper(){return au;}
  vector<T> getupper() const {return au;}
  pair<vector<T>,vector<T>> getAll(){return {al,au};}
  pair<vector<T>,vector<T>> getAll() const {return {al,au};}
};

}  // namespace soryuusi
