#include <cassert>
#include <limits>
#include <random>

#include "library/math/formal_power_series.hpp"
#include "library/math/modint.hpp"

namespace {

template<class T>
using FPS = soryuusi::FormalPowerSeries<T>;

template<class T>
FPS<T> product(const FPS<T>& f, const FPS<T>& g, int n){
  FPS<T> h(n);
  for(int i = 0; i < n && i < (int)f.size(); ++i)
    for(int j = 0; i+j < n && j < (int)g.size(); ++j) h[i+j] += f[i]*g[j];
  return h;
}

template<class T>
FPS<T> compose(const FPS<T>& f, const FPS<T>& g, int n){
  FPS<T> h(n);
  if(n == 0) return h;
  for(auto it = f.rbegin(); it != f.rend(); ++it){
    h = product(h,g,n);
    h[0] += *it;
  }
  return h;
}

template<class T>
void composition_family(){
  std::mt19937 gen(8193256);
  for(int n : {1,2,3,15,17,32,67,97}){
    FPS<T> f(n+5), g(n+3);
    for(auto& x : f) x = gen();
    for(auto& x : g) x = gen();
    for(int offset : {0,3}){
      g[0] = offset;
      for(int m : {0,1,n-1,n,n+3}){
        auto expected = compose(f,g,m);
        auto plan = soryuusi::static_composition<T>(g,m);
        assert(f.composition(g,m) == expected);
        assert(plan.composition(f) == expected);
        assert(plan.composition(f.pre(n)) == compose(f.pre(n),g,m));
        FPS<T> weights(m), p = f.pre(n);
        for(int i = 0; i < m; ++i){
          weights[i] = p[n-1];
          p = product(p,g,n);
        }
        assert(FPS<T>::PowerProjection(g,f,n,m) == weights);
      }
    }
    g[0] = 0; g[1] = 1;
    FPS<T> identity(n);
    if(n > 1) identity[1] = 1;
    const auto inverse = g.compositional_inverse(n);
    assert(compose(g,inverse,n) == identity && compose(inverse,g,n) == identity);

    for(int order : {0,1,2}){
      auto inner = g.pre(n);
      if(order == 0) inner.assign(n,T(0));
      if(order == 2 && n > 1) inner[1] = 0;
      const auto expected = compose(f.pre(n),inner,n);
      soryuusi::online_composition<T> plan(inner,n);
      for(int i = 0; i < n; ++i){
        if(i % 9 == 0){
          auto partial = compose(f.pre(i),inner,n);
          assert(plan.get(i) == partial[i]);
          assert(plan.get(n-1) == partial[n-1]);
        }
        assert(plan.query(f[i]) == expected[i]);
      }
    }
  }
  assert(FPS<T>().composition(FPS<T>(),0).empty());
}

template<class T>
void iterates(){
  using Fps = FPS<T>;
  for(int n : {1,2,4,7,19,38}){
    Fps before{0,2,3,5}, after{7,11,13,17}, weight{19,23,29,31};
    for(T linear : {T(0),T(1),T(3),T(-1)}){
      Fps f(n,1);
      f[0] = 0;
      if(n > 1) f[1] = linear;
      auto res = soryuusi::CompositionalPowerProjection(f,n,13,before,after,weight);
      auto g = after.pre(n);
      for(T value : res){
        assert(value == product(compose(g,before,n),weight,n)[n-1]);
        g = compose(g,f,n);
      }
    }
  }
}

template<class T>
void arithmetic(){
  std::mt19937 gen(18387);
  for(int n : {1,2,33,65,94,127,257,513}){
    FPS<T> f(n), g(n);
    for(auto& x : f) x = gen();
    for(auto& x : g) x = gen();
    f[0] = 0; g[0] = 1;
    auto exponential = f.exp(n);
    FPS<T> expected(n,1);
    for(int i = 1; i < n; ++i){
      T sum = 0;
      for(int j = 1; j <= i; ++j) sum += f[j]*T(j)*expected[i-j];
      expected[i] = sum/T(i);
    }
    assert(exponential == expected && exponential.log(n) == f);
    FPS<T> identity(n,0);
    identity[0] = 1;
    assert(product(g,g.inv(n),n) == identity);
    assert(product(g,g,n).sqrt(n) == g);
  }
  assert(FPS<T>().diff().empty());
  assert(FPS<T>({0,1}).pow(std::numeric_limits<long long>::max(),3) == FPS<T>(3,0));
}

}  // namespace

int main(){
  using Native = soryuusi::modint998244353;
  using CRT = soryuusi::modint1000000007;
  using Ring = soryuusi::static_modint<8>;
  using Wide = soryuusi::static_modint64<18446744073709551557ULL>;
  composition_family<Native>(); composition_family<CRT>();
  composition_family<soryuusi::static_modint<2>>(); composition_family<Ring>();
  composition_family<Wide>();
  iterates<Native>(); iterates<CRT>(); iterates<Ring>(); iterates<Wide>();
  arithmetic<Native>(); arithmetic<CRT>();
}
