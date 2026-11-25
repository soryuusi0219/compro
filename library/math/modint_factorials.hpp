// https://github.com/soryuusi0219/compro/blob/main/library/math/modint_factorials.hpp
#pragma once
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <map>
#include <type_traits>
#include <utility>
#include <vector>

#include "internal/internal_math.hpp"
#include "internal/modint_traits.hpp"

namespace soryuusi {

namespace internal {

template<class T>
std::pair<T,std::vector<T>> factorial_and_inverse_factorials(int n){
  T fact = 1;
  for(int i = 2; i <= n; ++i) fact *= T(i);
  std::vector<T> inverse(n+1);
  inverse[n] = fact.inv();
  for(int i = n; i > 0; --i) inverse[i-1] = inverse[i]*T(i);
  return {fact,std::move(inverse)};
}

template<class UInt>
struct FactorialValues {
  std::vector<UInt> invs, facts, inv_facts;
  int max_calculated = -1;

  template<class Multiply>
  void extend(int n, std::uint64_t mod, Multiply mul){
    if(n <= max_calculated) return;
    assert(static_cast<std::uint64_t>(n) < mod);
    invs.resize(std::size_t(n)+1);
    facts.resize(std::size_t(n)+1);
    inv_facts.resize(std::size_t(n)+1);
    if(max_calculated < 0){
      invs[0] = 0;
      invs[1] = facts[0] = facts[1] = inv_facts[0] = inv_facts[1] = 1;
    }
    const std::size_t end = n;
    for(std::size_t i = max_calculated < 0 ? 2 : std::size_t(max_calculated)+1; i <= end; ++i){
      const UInt inverse = mul(invs[mod%i],mod/i);
      invs[i] = inverse ? mod-inverse : 0;
      facts[i] = mul(facts[i-1],i);
      inv_facts[i] = mul(inv_facts[i-1],invs[i]);
    }
    max_calculated = n;
  }
};

struct FactorialCache {
  const std::uint64_t mod;
  const barrett reducer;
  FactorialValues<std::uint32_t> compact;
  FactorialValues<std::uint64_t> wide;

  explicit FactorialCache(std::uint64_t modulus)
      : mod(modulus), reducer(mod <= std::numeric_limits<int>::max() ? mod : 1) {}

  static constexpr std::uint64_t compact_limit = std::numeric_limits<int>::max();

  const auto& get_compact(int n){
    compact.extend(n,mod,[&](std::uint32_t a, std::uint32_t b){return reducer.mul(a,b);});
    return compact;
  }

  const auto& get_wide(int n){
    wide.extend(n,mod,[&](std::uint64_t a, std::uint64_t b){return mul_mod64(a,b,mod);});
    return wide;
  }
};

inline FactorialCache& factorial_cache(std::uint64_t mod){
  static std::map<std::uint64_t,FactorialCache> caches;
  return caches.try_emplace(mod,mod).first->second;
}

}  // namespace internal

using namespace std;

template<class T>
class modint_factorials{
private:
  static internal::FactorialCache& cache(){
    if constexpr (internal::is_static_modint_like_v<T>){
      static auto& table = internal::factorial_cache(T::mod());
      return table;
    }else{
      static internal::FactorialCache* table = nullptr;
      const std::uint64_t mod = T::mod();
      if(!table || table->mod != mod) table = &internal::factorial_cache(mod);
      return *table;
    }
  }

  template<class F>
  static decltype(auto) with_table(int n, F use){
    assert(n >= 0);
    auto& table = cache();
    if(n < 1) n = 1;
    if constexpr (internal::is_static_modint_like_v<T>){
      if constexpr (T::mod() <= internal::FactorialCache::compact_limit) return use(table.get_compact(n));
      else return use(table.get_wide(n));
    }else if constexpr (sizeof(T::mod()) < sizeof(std::uint64_t) && std::is_signed_v<decltype(T::mod())>){
      return use(table.get_compact(n));
    }else{
      if(table.mod <= internal::FactorialCache::compact_limit) return use(table.get_compact(n));
      return use(table.get_wide(n));
    }
  }

  static T from_residue(std::uint64_t x){
    if constexpr (requires { T::raw(x); }) return T::raw(x);
    else return T(x);
  }

public:
  modint_factorials() = default;
  modint_factorials(int n) {precompute(n);}

  void precompute(int n) {with_table(n,[](const auto&){});}

  T mint_inv(int x) {
    assert(x != 0);
    if(x < 0) return -mint_inv(-x);
    return with_table(x,[&](const auto& table){return from_residue(table.invs[x]);});
  }
  T mint_factorial(int x) {
    return with_table(x,[&](const auto& table){return from_residue(table.facts[x]);});
  }
  T mint_inv_factorial(int x) {
    return with_table(x,[&](const auto& table){return from_residue(table.inv_facts[x]);});
  }

  inline T inv(int x) {return mint_inv(x);}
  inline T fact(int x) {return mint_factorial(x);}
  inline T ifact(int x) {return mint_inv_factorial(x);}

  // n * (n-1) * ... * (n-k+1)
  template<bool admit_negative = false>
  T nPk(int n, int k) {
    if(n >= 0 && k > n) return T(0);
    if constexpr (admit_negative){
      if(k < 0) return T(0);
      if(n < 0) return ((k & 1) ? -nPk<false>(-n+k-1,k) : nPk<false>(-n+k-1,k));
    }else{
      assert(n >= 0 && k >= 0);
    }
    return with_table(n,[&](const auto& table){
      return from_residue(table.facts[n]) * from_residue(table.inv_facts[n-k]);
    });
  }
  template<bool admit_negative = false>
  T nPk_inv(int n, int k) {
    assert(k >= 0);
    if constexpr (admit_negative){
      if(n < 0) return ((k & 1) ? -nPk_inv<false>(-n+k-1,k) : nPk_inv<false>(-n+k-1,k));
    }else{
      assert(n >= 0);
    }
    assert(k <= n);
    return with_table(n,[&](const auto& table){
      return from_residue(table.inv_facts[n]) * from_residue(table.facts[n-k]);
    });
  }
  // [X^k](1+X)^n
  template<bool admit_negative = false>
  T nCk(int n, int k) {
    if constexpr (admit_negative){
      if(k < 0) return T(0);
      if(n < 0) return ((k & 1) ? -nCk<false>(-n+k-1,k) : nCk<false>(-n+k-1,k));
      if(n >= 0 && k > n) return T(0);
    }else{
      assert(k >= 0 && k <= n);
    }
    return with_table(n,[&](const auto& table){
      return from_residue(table.facts[n]) * from_residue(table.inv_facts[k]) * from_residue(table.inv_facts[n-k]);
    });
  }
  template<bool admit_negative = false>
  T nCk_inv(int n, int k) {
    assert(k >= 0);
    if constexpr (admit_negative){
      if(n < 0) return ((k & 1) ? -nCk_inv<false>(-n+k-1,k) : nCk_inv<false>(-n+k-1,k));
    }else{
      assert(n >= 0);
    }
    assert(k <= n);
    return with_table(n,[&](const auto& table){
      return from_residue(table.inv_facts[n]) * from_residue(table.facts[k]) * from_residue(table.facts[n-k]);
    });
  }
  // Catalan number
  // C_n = 1/(n+1) nCk(2n,n)
  T Catalan(int n) {
    assert(n >= 0);
    return with_table(n*2,[&](const auto& table){
      return from_residue(table.facts[n*2]) * from_residue(table.inv_facts[n]) * from_residue(table.inv_facts[n+1]);
    });
  }
  // [X^n]((1-sqrt(1-4X))/2X)^k
  T Catalan_convolution(int n, int k) {
    assert(n >= 0 && k >= 0);
    if(k == 0) return n ? T(0) : T(1);
    if(n == 0) return T(1);
    return with_table(n*2+k-1,[&](const auto& table){
      return from_residue(table.facts[n*2+k-1]) * from_residue(table.inv_facts[n+k])
        * from_residue(table.inv_facts[n]) * T(k);
    });
  }
  // path from (0,0) to (n,m) with y <= x+k
  T Path_lower(int n, int m, int k) {
    assert(n >= 0 && m >= 0);
    if(k < 0 || m > n+k) return T(0);
    if(k >= m) return nCk(n+m,n);
    return nCk(n+m,n) - nCk(n+m,n+k+1);
  }
  // path from (0,0) to (n,m) with x+l <= y <= x+k
  T Path_between(int n, int m, int l, int k) {
    assert(n >= 0 && m >= 0);
    if(l > 0 || k < 0 || m > n+k || m < n+l) return T(0);
    if(l == k) return n || m ? T(0) : T(1); // l = k = 0
    int p = k - l + 2;
    T res = 0;
    int r1 = m % p;
    int r2 = (m-l+1) % p;
    for(int x = r1; x <= n+m; x += p) res += nCk(n+m, x);
    for(int x = r2; x <= n+m; x += p) res -= nCk(n+m, x);
    return res;
  }
  // Narayana number
  // N(n,k) = 1/n nCr(n,k) nCr(n,k-1)
  // parenthesis sequence of length 2n with k peaks
  T Narayana(int n, int k) {
    assert(k >= 0 && n >= k);
    if(n == 0) return T(1);
    if(k == 0) return T(0);
    return with_table(n,[&](const auto& table){
      return from_residue(table.facts[n]) * from_residue(table.facts[n-1])
        * from_residue(table.inv_facts[k]) * from_residue(table.inv_facts[k-1])
        * from_residue(table.inv_facts[n-k]) * from_residue(table.inv_facts[n-k+1]);
    });
  }
};

}  // namespace soryuusi

// modint_factorials<mint> mf(1'000'000);
