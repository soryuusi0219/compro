#include <cassert>
#include <bit>
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

template<class T>
void polynomial_prefix_sums(){
  std::mt19937 gen(944);
  assert(FPS<T>().prefix_sum().empty());
  for(int n : {1,2,3,31,32,65,128,513}){
    FPS<T> polynomial(n);
    for(T& x : polynomial) x = gen();
    const FPS<T> sums = polynomial.prefix_sum();
    assert(sums.size() == polynomial.size()+1 && sums[0] == T(0));
    const auto difference = sums.Taylor_shift(T(1))-sums;
    assert(difference.pre(n) == polynomial && difference[n] == T(0));
    T expected = 0;
    for(int i = 0; i < n+5; ++i){
      assert(sums.eval(T(i)) == expected);
      expected += polynomial.eval(T(i));
    }
  }
}

void shortest_recurrences(){
  using Fps = FPS<soryuusi::static_modint<2>>;
  for(int n = 0; n <= 11; ++n){
    for(unsigned int bits = 0; bits < (1U << n); ++bits){
      Fps samples(n);
      for(int i = 0; i < n; ++i) samples[i] = (bits >> i)&1;
      const auto recurrence = Fps::find_linear_recurrence(samples);
      const int d = recurrence.size();
      unsigned int mask = 0;
      for(auto c : recurrence) mask = (mask << 1)|c.val();
      auto fits = [=](int order, unsigned int coefficients){
        for(int i = order; i < n; ++i)
          if((std::popcount((bits >> (i-order))&coefficients)&1) != ((bits >> i)&1)) return false;
        return true;
      };
      assert(d <= n && fits(d,mask));
      for(int order = 0; order < d; ++order)
        for(unsigned int c = 0; c < (1U << order); ++c) assert(!fits(order,c));
    }
  }
  const Fps impulse{1,0,0};
  assert(Fps::find_linear_recurrence(impulse) == Fps{0});
  assert(Fps::linear_recurrence_terms(impulse,0,5) == Fps({1,0,0,0,0}));
  assert(Fps::linear_recurrence_terms(Fps{},100,3) == Fps(3));
  assert(Fps::kth_term_of_linear_recurrence(Fps{},100) == 0);
}

template<class T, bool Infer>
void recurrence_terms(){
  using Fps = FPS<T>;
  std::mt19937 gen(537);
  for(int d : {1,2,3,16,33,65,79,128}){
    Fps initial(d), recurrence(d);
    for(T& x : initial) x = gen();
    for(T& x : recurrence) x = gen();
    recurrence.back() = 0;
    Fps expected = initial;
    for(int i = d; i < 12*d+27; ++i){
      T next = 0;
      for(int j = 0; j < d; ++j) next += expected[i-1-j]*recurrence[j];
      expected.push_back(next);
    }
    const auto samples = expected.pre(2*d+1);
    for(int start : {0,d-1,d,d+1,4*d+7,9*d}){
      assert(Fps::kth_term_of_linear_recurrence(initial,recurrence,start) == expected[start]);
      if constexpr (Infer) assert(Fps::kth_term_of_linear_recurrence(samples,start) == expected[start]);
      for(int count : {0,1,2,d,d+13}){
        const Fps range(expected.begin()+start,expected.begin()+start+count);
        assert(Fps::linear_recurrence_terms(initial,recurrence,start,count) == range);
        if constexpr (Infer) assert(Fps::linear_recurrence_terms(samples,start,count) == range);
      }
    }

    recurrence.assign(d,T(0));
    recurrence.back() = 1;
    constexpr long long start = std::numeric_limits<long long>::max();
    Fps periodic(d+3), repeated(2*d);
    for(int i = 0; i < d+3; ++i) periodic[i] = initial[(start%d+i)%d];
    for(int i = 0; i < 2*d; ++i) repeated[i] = initial[i%d];
    assert(Fps::linear_recurrence_terms(initial,recurrence,start,d+3) == periodic);
    assert(Fps::kth_term_of_linear_recurrence(initial,recurrence,start) == periodic[0]);
    if constexpr (Infer){
      assert(Fps::linear_recurrence_terms(repeated,start,d+3) == periodic);
      assert(Fps::kth_term_of_linear_recurrence(repeated,start) == periodic[0]);
    }
  }
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
  polynomial_prefix_sums<Native>(); polynomial_prefix_sums<CRT>();
  shortest_recurrences();
  recurrence_terms<Native,true>(); recurrence_terms<CRT,true>();
  recurrence_terms<Ring,false>();
}
