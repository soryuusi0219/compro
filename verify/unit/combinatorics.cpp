#include <cassert>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <random>
#include <set>
#include <string>
#include <vector>

#include "library/math/combinatorics.hpp"
#include "library/math/formal_power_series.hpp"
#include "library/math/modint.hpp"
#include "library/math/number_of_subsequences.hpp"

namespace {

template<class T>
void tables(){
  constexpr int n = 100;
  std::vector<std::vector<T>> first(n+1,std::vector<T>(n+1)), second = first;
  first[0][0] = second[0][0] = 1;
  for(int i = 1; i <= n; ++i){
    for(int k = 1; k <= i; ++k){
      first[i][k] = first[i-1][k-1]-T(i-1)*first[i-1][k];
      second[i][k] = second[i-1][k-1]+T(k)*second[i-1][k];
    }
  }
  const auto bell = soryuusi::bell_numbers<T>(n);
  for(int i : {0,1,2,3,12,63,64,95,100}){
    const auto row1 = soryuusi::stirling_first_kind<T>(i);
    const auto row2 = soryuusi::stirling_second_kind<T>(i);
    T sum = 0;
    for(int k = 0; k <= i; ++k){
      assert(row1[k] == first[i][k] && row2[k] == second[i][k]);
      sum += second[i][k];
    }
    assert(bell[i] == sum);
  }
  for(int k : {0,1,2,3,47,60,99,100}){
    const auto column1 = soryuusi::stirling_first_kind_fixed_k<T>(n,k);
    const auto column2 = soryuusi::stirling_second_kind_fixed_k<T>(n,k);
    for(int i = k; i <= n; ++i)
      assert(column1[i-k] == first[i][k] && column2[i-k] == second[i][k]);
  }

  std::vector<T> partitions(n+1);
  partitions[0] = 1;
  for(int part = 1; part <= n; ++part)
    for(int i = part; i <= n; ++i) partitions[i] += partitions[i-part];
  assert(soryuusi::partition_numbers<T>(n) == partitions);

  const auto bernoulli = soryuusi::bernoulli_numbers<T>(n);
  soryuusi::modint_factorials<T> factorials(n+1);
  assert(bernoulli[0] == T(1) && bernoulli[1] == -T(2).inv());
  assert(soryuusi::bernoulli_numbers<T>(n-1) == std::vector<T>(bernoulli.begin(),bernoulli.end()-1));
  for(int i = 1; i <= n; ++i){
    T sum = 0;
    for(int j = 0; j <= i; ++j) sum += factorials.nCk(i+1,j)*bernoulli[j];
    assert(sum == T(0));
  }
}

template<class T>
void factorials(){
  constexpr int n = 380000;
  std::vector<T> expected(n+1,T(1));
  for(int i = 1; i <= n; ++i) expected[i] = expected[i-1]*T(i);
  std::vector<long long> queries{0,1,2,32768,1<<16,n,n-1,n,T::mod(),(long long)T::mod()+1};
  for(int i = 0; i <= 6; ++i) queries.push_back((long long)T::mod()-1-i);
  std::mt19937 gen(4551);
  for(int i = 0; i < 800; ++i) queries.push_back(gen()%(n+1));
  const auto actual = soryuusi::many_factorials<T>(queries);
  for(std::size_t i = 0; i < queries.size(); ++i){
    const auto index = queries[i];
    if(index < n+1) assert(actual[i] == expected[index]);
    else if(index >= T::mod()) assert(actual[i] == T(0));
    else {
      T inverse = expected[T::mod()-1-index].inv();
      assert(actual[i] == ((index&1) ? inverse : -inverse));
    }
  }
  assert(soryuusi::factorial<T>(n) == expected[n]);
}

void montmort(){
  for(std::uint64_t mod : {std::uint64_t(1),std::uint64_t(12),std::numeric_limits<std::uint64_t>::max()}){
    const auto values = soryuusi::montmort_numbers(120,mod);
    assert(values.front() == 0);
    std::uint64_t previous = 1%mod, current = 0;
    for(int n = 2; n <= 120; ++n){
      const std::uint64_t next = ((__uint128_t(previous)+current)*(n-1))%mod;
      assert(values[n-1] == next);
      previous = current;
      current = next;
    }
  }
}

template<class T>
void weighted_sums(){
  using FPS = soryuusi::FormalPowerSeries<T>;
  std::mt19937 gen(993);
  for(int size : {1,2,7,22,130}){
    FPS f(size);
    for(auto& x : f) x = gen();
    std::vector<T> values(size);
    for(int i = 0; i < size; ++i) values[i] = f.eval(T(i));
    for(T r : {T(0),T(1),T(2),T(-1),T(47)}){
      for(int n : {0,1,size-1,size,size+1,2*size+11}){
        T sum = 0, power = 1;
        for(int i = 0; i < n; ++i){
          sum += power*f.eval(T(i));
          power *= r;
        }
        assert(FPS::sum_of_exponential_times_polynomial(values,r,n) == sum);
        assert(f.sum_of_exponential_times_polynomial(r,n) == sum);
      }
      if(r == T(1)) continue;
      soryuusi::modint_factorials<T> factorials(size);
      T numerator = 0;
      for(int i = size-1; i >= 0; --i){
        T coefficient = 0;
        for(int j = 0; j <= i; ++j){
          T value = factorials.nCk(size,j)*values[i-j];
          coefficient += j&1 ? -value : value;
        }
        numerator = numerator*r+coefficient;
      }
      const T expected = numerator/(T(1)-r).pow(size);
      assert(FPS::sum_of_exponential_times_polynomial_limit(values,r) == expected);
      assert(f.sum_of_exponential_times_polynomial_limit(r) == expected);

      const long long n = std::numeric_limits<long long>::max()-31;
      assert(f.sum_of_exponential_times_polynomial(r,n)-f.sum_of_exponential_times_polynomial(r,n-1)
             == r.pow(n-1)*f.eval(T(n-1)));
    }
  }
  assert(FPS{}.sum_of_exponential_times_polynomial(T(1),10) == 0);
  assert(FPS{}.sum_of_exponential_times_polynomial_limit(T(2)) == 0);
}

void shared_factorials(){
  using Dynamic = soryuusi::dynamic_modint<1>;
  using Other = soryuusi::dynamic_modint64<2>;
  using Static = soryuusi::static_modint<17>;
  soryuusi::modint_factorials<Static> shared(16);
  soryuusi::modint_factorials<Dynamic> first;
  soryuusi::modint_factorials<Other> other;
  Other::set_mod(17);
  assert(other.fact(10).val() == shared.fact(10).val());
  for(int p : {17,19,17}){
    Dynamic::set_mod(p);
    soryuusi::modint_factorials<Dynamic> second(8);
    auto copied = first;
    second.precompute(p-1);
    assert(first.fact(p-1) == Dynamic(-1));
    assert(first.ifact(8)*second.fact(8) == 1);
    assert(first.inv(-3)*Dynamic(-3) == 1);
    assert(first.nPk(8,3) == Dynamic(336));
    assert(copied.nPk_inv(8,3)*second.nPk(8,3) == 1);
    assert(first.nCk(8,3) == Dynamic(56));
    assert(first.nCk_inv(8,3)*copied.nCk(8,3) == 1);
    assert(first.nCk<true>(-4,3) == Dynamic(-20));
    assert(first.Catalan(4) == Dynamic(14));
    assert(first.Catalan_convolution(3,2) == Dynamic(14));
    assert(first.Narayana(5,2) == Dynamic(10));
    const std::vector<Dynamic> subset{2,4,4,4,2,0,0};
    assert(soryuusi::sharp_p_subset_sum<Dynamic>({0,1,1,2,10},6) == subset);
  }
  Other::set_mod(18446744073709551557ULL);
  const auto fact = other.fact(18);
  assert(fact == Other(6402373705728000ULL) && other.ifact(18)*fact == 1);
  Other::set_mod(17);
  assert(other.fact(16).val() == shared.fact(16).val());
}

void subset_and_subsequence_counts(){
  using Mint = soryuusi::modint998244353;
  std::mt19937 gen(7594);
  assert(soryuusi::sharp_p_subset_sum<Mint>({},0) == std::vector<Mint>{Mint(1)});
  assert(soryuusi::number_of_subsequences<int>(std::vector<std::string>{"a","b","a"}) == 6);
  for(int n = 0; n <= 10; ++n){
    for(int trial = 0; trial < 12; ++trial){
      std::vector<int> values(n);
      for(int& x : values) x = gen()%7;
      const int limit = gen()%25;
      std::vector<Mint> subsets(limit+1);
      std::set<std::vector<int>> subsequences;
      for(int mask = 0; mask < (1<<n); ++mask){
        int sum = 0;
        std::vector<int> sequence;
        for(int i = 0; i < n; ++i){
          if((mask>>i)&1){sum += values[i]; sequence.push_back(values[i]);}
        }
        if(sum <= limit) subsets[sum] += 1;
        if(!sequence.empty()) subsequences.insert(sequence);
      }
      assert(soryuusi::sharp_p_subset_sum<Mint>(values,limit) == subsets);
      assert(soryuusi::number_of_subsequences<Mint>(values) == Mint(subsequences.size()));
    }
  }
}

}  // namespace

int main(){
  using Native = soryuusi::modint998244353;
  using CRT = soryuusi::modint1000000007;
  using Composite = soryuusi::static_modint<8>;
  using Dynamic = soryuusi::dynamic_modint<0>;
  tables<Native>(); tables<CRT>();
  factorials<Native>(); factorials<CRT>();
  Dynamic::set_mod(1000003);
  factorials<Dynamic>();
  Dynamic::set_mod(12);
  assert(soryuusi::factorial<Dynamic>(3) == Dynamic(6));
  const std::vector<int> small{0,1,2,3,4,7,8};
  assert(soryuusi::many_factorials<Composite>(small) == std::vector<Composite>({1,1,2,6,0,0,0}));
  montmort();
  weighted_sums<Native>(); weighted_sums<CRT>();
  shared_factorials();
  subset_and_subsequence_counts();
  using Mod9 = soryuusi::static_modint<9>;
  const soryuusi::FormalPowerSeries<Mod9> f{1,1};
  assert(f.sum_of_exponential_times_polynomial(Mod9(3),100) == Mod9(7));
  assert(f.sum_of_exponential_times_polynomial_limit(Mod9(3)) == Mod9(7));
}
