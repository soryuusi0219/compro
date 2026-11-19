#include <algorithm>
#include <bit>
#include <cassert>
#include <cstddef>
#include <limits>
#include <random>
#include <vector>

#include "library/math/convolution_subset.hpp"
#include "library/math/modint.hpp"
#include "library/math/set_power_series.hpp"

namespace {

template<class T>
using SPS = soryuusi::SetPowerSeries<T>;

template<class T>
T coefficient(const std::vector<T>& a, const std::vector<T>& b, std::size_t mask){
  T result = 0;
  for(std::size_t s = mask;; s = (s-1)&mask){
    if(s < a.size() && (mask^s) < b.size()) result += a[s]*b[mask^s];
    if(s == 0) break;
  }
  return result;
}

template<class T>
SPS<T> naive_product(const std::vector<T>& a, const std::vector<T>& b){
  if(a.empty() || b.empty()) return {};
  SPS<T> c(std::bit_ceil(std::max(a.size(),b.size())));
  for(std::size_t s = 0; s < c.size(); ++s) c[s] = coefficient(a,b,s);
  return c;
}

template<class T>
void check_convolutions(){
  std::mt19937_64 random(302);
  for(std::size_t n : {1,2,5,24,32,63,64,65,110,128,129,511,2048}){
    std::vector<T> f(n), g(n/2+1);
    for(T& x : f) x = random()%64;
    for(T& x : g) x = random()%64;
    assert(soryuusi::convolution_subset(f,g) == naive_product(f,g));
    assert(soryuusi::convolution_subset(g,f) == naive_product(g,f));
    assert(soryuusi::convolution_subset(f,f) == naive_product(f,f));
    assert(soryuusi::convolution_subset(f,std::vector<T>{T(2)}) == naive_product(f,std::vector<T>{T(2)}));
    assert(soryuusi::convolution_subset(f,std::vector<T>{}).empty());
  }
  const std::vector<T> three{T(1),T(2),T(3)};
  const SPS<T> padded{T(1),T(2),T(3),T(0)};
  assert(SPS<T>(three) == padded && SPS<T>(std::vector<T>(three)) == padded);
}

template<class T>
void check_series(bool has_half){
  std::mt19937_64 random(38192);
  for(int n : {0,1,4,7,8,9}){
    const int size = 1 << n;
    SPS<T> f(size), g(size), one(size);
    for(T& x : f) x = random();
    for(T& x : g) x = random();
    g[0] = one[0] = T(1);
    f[0] = T(0);

    SPS<T> exponential(size);
    exponential[0] = T(1);
    for(int s = 1; s < size; ++s){
      const int bit = s&-s;
      for(int t = s; t; t = (t-1)&s)
        if(t&bit) exponential[s] += f[t]*exponential[s^t];
    }
    assert(f.exp() == exponential);
    assert(exponential.log() == f);
    const auto inverse = g.inv();
    assert(naive_product(g,inverse) == one);
    assert(f/g == naive_product(f,inverse));
    auto alias = g;
    alias /= alias;
    assert(alias == one);
    if(n > 1){
      const SPS<T> prefix(g.begin(),g.begin()+size/4);
      assert(f/prefix == naive_product(f,prefix.inv()));
      assert(prefix/g == naive_product(prefix,inverse));
    }
    assert(naive_product(g,g.pow(-1)) == one);
    assert(g.pow(-3) == naive_product(naive_product(inverse,inverse),inverse));
    assert(g.pow(std::numeric_limits<int>::min()) == g.pow(std::numeric_limits<int>::min()/2).pow(2));
    if(has_half){
      const auto root = g.sqrt();
      assert(root[0] == T(1) && naive_product(root,root) == g);
    }

    for(T constant : {T(0),T(1),T(2)}){
      f[0] = constant;
      SPS<T> power = one, composite(size), low_degree(size);
      std::vector<T> polynomial(n+4), projections;
      for(auto& x : polynomial) x = random();
      for(int i = 0; i < (int)polynomial.size(); ++i){
        projections.push_back(coefficient(power,g,size-1));
        for(int s = 0; s < size; ++s){
          composite[s] += polynomial[i]*power[s];
          if(i < 2) low_degree[s] += polynomial[i]*power[s];
        }
        assert(f.pow(i) == power);
        power = naive_product(power,f);
      }
      assert(soryuusi::composition(polynomial,f) == composite);
      assert(soryuusi::composition(std::vector<T>{polynomial[0],polynomial[1]},f) == low_degree);
      assert(soryuusi::composition(std::vector<T>{T(1)},f) == one);
      assert(soryuusi::composition(std::vector<T>{},f) == SPS<T>(size));
      assert(soryuusi::power_projection(f,g,projections.size()) == projections);
      assert(soryuusi::power_projection(f,g,1) == std::vector<T>{projections[0]});
      assert(soryuusi::power_projection(f,g,2) == std::vector<T>(projections.begin(),projections.begin()+2));
      assert(soryuusi::power_projection(f,g,0).empty());
    }
  }
}

}  // namespace

int main(){
  using Mint = soryuusi::modint998244353;
  using Small = soryuusi::static_modint<2>;
  using Ring = soryuusi::static_modint<8>;
  using Dynamic = soryuusi::dynamic_modint<0>;
  using Wide = soryuusi::static_modint64<18446744073709551557ULL>;
  Dynamic::set_mod(2147483647);
  check_convolutions<long long>();
  check_convolutions<Mint>();
  check_series<Mint>(true);
  check_series<Small>(false);
  check_series<Ring>(false);
  check_series<Dynamic>(true);
  check_series<Wide>(true);
}
