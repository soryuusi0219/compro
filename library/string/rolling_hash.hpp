// https://github.com/soryuusi0219/compro/blob/main/library/string/rolling_hash.hpp
#pragma once
#include <algorithm>
#include <cassert>
#include <cstdint>
#include <random>
#include <string>
#include <type_traits>
#include <vector>

namespace soryuusi {

using namespace std;

struct RolHashMint {
  using u64 = uint64_t;
  using u128 = __uint128_t;
  using mint = RolHashMint;
  static constexpr u64 MOD = (u64{1} << 61) - 1;
  static constexpr u64 MASK30 = (u64{1} << 30) - 1;
  static constexpr u64 MASK31 = (u64{1} << 31) - 1;
  static constexpr u64 MASK61 = MOD;
  static constexpr u64 POSITIVISER = MOD << 2;
  static constexpr u64 take_mod(u64 x){
    x = (x&MASK61) + (x >> 61);
    if(x >= MOD) x -= MOD;
    return x;
  }
  static constexpr u64 mul(u64 a, u64 b){
    u64 au = a >> 31;
    u64 ad = a & MASK31;
    u64 bu = b >> 31;
    u64 bd = b & MASK31;
    u64 mid = ad * bu + au * bd;
    u64 midu = mid >> 30;
    u64 midd = mid & MASK30;
    return au * bu * 2 + midu + (midd << 31) + ad * bd;
  }

  u64 v;
  constexpr RolHashMint() : v(0) {}
  template<class T, std::enable_if_t<std::is_integral_v<T>>* = nullptr>
  constexpr RolHashMint(T v_) {
    if constexpr (std::is_signed_v<T>) {
      __int128_t x = static_cast<__int128_t>(v_) % static_cast<__int128_t>(MOD);
      if (x < 0) x += MOD;
      v = static_cast<u64>(x);
    }else{
      v = static_cast<u64>(static_cast<u128>(v_) % MOD);
    }
  }
  static constexpr mint raw(u64 v_){mint x; x.v = v_; return x;}
  constexpr u64 val() const {return v;}
  constexpr mint& operator++() {if (++v == MOD) v = 0; return *this;}
  constexpr mint& operator--() {if (v == 0) v = MOD; --v; return *this;}
  constexpr mint operator++(int) {mint result = *this;++*this;return result;}
  constexpr mint operator--(int) {mint result = *this;--*this;return result;}
  constexpr mint& operator+=(const mint& rhs) {v += rhs.v; if (v >= MOD) v -= MOD; return *this;}
  constexpr mint& operator-=(const mint& rhs) {v += MOD-rhs.v; if (v >= MOD) v -= MOD; return *this;}
  constexpr mint& operator*=(const mint& rhs) {v = take_mod(mul(v,rhs.v)); return *this;}
  friend constexpr mint operator+(mint lhs, const mint& rhs) { return lhs += rhs; }
  friend constexpr mint operator-(mint lhs, const mint& rhs) { return lhs -= rhs; }
  friend constexpr mint operator*(mint lhs, const mint& rhs) { return lhs *= rhs; }
  friend constexpr bool operator==(const mint& lhs, const mint& rhs) {return lhs.v == rhs.v;}
  friend constexpr bool operator!=(const mint& lhs, const mint& rhs) {return lhs.v != rhs.v;}
  friend constexpr bool operator<(const mint& lhs, const mint& rhs) {return lhs.v < rhs.v;}
  friend constexpr bool operator>(const mint& lhs, const mint& rhs) {return lhs.v > rhs.v;}
  friend constexpr bool operator<=(const mint& lhs, const mint& rhs) {return lhs.v <= rhs.v;}
  friend constexpr bool operator>=(const mint& lhs, const mint& rhs) {return lhs.v >= rhs.v;}
  static constexpr mint muladd(const mint a, const mint b, const mint c){
    const u64 product = take_mod(mul(a.v,b.v));
    return raw(product + c.v >= MOD ? product + c.v - MOD : product + c.v);
  }
  static constexpr mint mulsub(const mint a, const mint b, const mint c){
    const u64 product = take_mod(mul(a.v,b.v));
    return raw(product >= c.v ? product - c.v : product + MOD - c.v);
  }
  static constexpr mint submul(const mint a, const mint b, const mint c){
    const u64 product = take_mod(mul(b.v,c.v));
    return raw(a.v >= product ? a.v - product : a.v + MOD - product);
  }
};

template<int N>
struct RolHashValue{
  using Val = RolHashValue;
  static_assert(N > 0);
  using mint = RolHashMint;
  mint val[N];
  constexpr RolHashValue() : val{} {}
  template<class T>
  constexpr RolHashValue(const T& x) : val{} {for(int i = 0; i < N; ++i) val[i] = mint(x);}
  static constexpr Val raw(uint64_t x){Val res; for(int i = 0; i < N; ++i) {res[i] = mint::raw(x);} return res;}
  constexpr mint& operator[](int i) {return val[i];}
  constexpr const mint& operator[](int i) const {return val[i];}
  constexpr Val& operator+=(const Val& rhs) {for(int i = 0; i < N; ++i) {val[i] += rhs[i];} return *this;}
  constexpr Val& operator-=(const Val& rhs) {for(int i = 0; i < N; ++i) {val[i] -= rhs[i];} return *this;}
  constexpr Val& operator*=(const Val& rhs) {for(int i = 0; i < N; ++i) {val[i] *= rhs[i];} return *this;}
  friend constexpr Val operator+(Val lhs, const Val& rhs) {return lhs += rhs;}
  friend constexpr Val operator-(Val lhs, const Val& rhs) {return lhs -= rhs;}
  friend constexpr Val operator*(Val lhs, const Val& rhs) {return lhs *= rhs;}
  friend constexpr bool operator==(const Val& lhs, const Val& rhs) {for(int i = 0; i < N; ++i) {if(lhs[i] != rhs[i]) return false;} return true;}
  friend constexpr bool operator!=(const Val& lhs, const Val& rhs) {return !(lhs == rhs);}
  friend constexpr bool operator<(const Val& lhs, const Val& rhs) {for(int i = 0; i < N; ++i) {if(lhs[i] != rhs[i]) return lhs[i] < rhs[i];} return false;}
  friend constexpr bool operator>(const Val& lhs, const Val& rhs) {return rhs < lhs;}
  friend constexpr bool operator<=(const Val& lhs, const Val& rhs) {return !(rhs < lhs);}
  friend constexpr bool operator>=(const Val& lhs, const Val& rhs) {return !(lhs < rhs);}
  static constexpr Val muladd(const Val& a, const Val& b, const Val& c) {Val res; for(int i = 0; i < N; ++i) {res[i] = mint::muladd(a[i], b[i], c[i]);} return res;}
  static constexpr Val mulsub(const Val& a, const Val& b, const Val& c) {Val res; for(int i = 0; i < N; ++i) {res[i] = mint::mulsub(a[i], b[i], c[i]);} return res;}
  static constexpr Val submul(const Val& a, const Val& b, const Val& c) {Val res; for(int i = 0; i < N; ++i) {res[i] = mint::submul(a[i], b[i], c[i]);} return res;}
};

template<int N = 1, int id = -1>
struct RollingHash{
  private:
    static_assert(N > 0);
    using mint = RolHashMint;
    using Hash = std::conditional_t<N == 1, mint, RolHashValue<N>>;
    static Hash make_base() {
      std::random_device seed;
      std::seed_seq seeds{seed(), seed(), seed(), seed()};
      mt19937_64 mt(seeds);
      std::uniform_int_distribution<uint64_t> distribution(256, mint::MOD - 2);
      Hash result;
      if constexpr (N == 1) {
        result = mint::raw(distribution(mt));
      }else{
        for(int i = 0; i < N; i++) result[i] = mint::raw(distribution(mt));
      }
      return result;
    }
    inline static const Hash base = make_base();
    int n;
    vector<Hash> base_pow;
    vector<Hash> data;
  public:
    RollingHash() : n(0), base_pow(1, Hash::raw(1)), data(1, Hash::raw(0)) {}
    template<class T>
    explicit RollingHash(const T& v) : n(static_cast<int>(v.size())), base_pow(n+1), data(n+1){
      base_pow[0] = Hash::raw(1);
      data[0] = Hash::raw(0);
      for(int i = 0; i < n; ++i){
        base_pow[i+1] = base_pow[i] * base;
        data[i+1] = Hash::muladd(data[i],base,Hash(v[i]));
      }
    }
    inline int size() const {return n;}
    template<class T> void push_back(T x){
      base_pow.push_back(base_pow[n] * base);
      data.push_back(Hash::muladd(data[n],base,Hash(x)));
      n++;
    }
    Hash get_hash(int l, int r) const {
      assert(0 <= l && l <= r && r <= n);
      return Hash::submul(data[r],data[l],base_pow[r-l]);
    }
    bool is_same(int l1, int r1, int l2, int r2) const {
      return r1-l1 == r2-l2 && get_hash(l1,r1) == get_hash(l2,r2);
    }
    bool is_same(int l1, int r1, int l2, int r2, const RollingHash& y) const {
      return r1-l1 == r2-l2 && get_hash(l1,r1) == y.get_hash(l2,r2);
    }
    int lcp(int l1, int l2) const {
      assert(0 <= l1 && l1 <= n && 0 <= l2 && l2 <= n);
      int ok = 0; int ng = min(n-l1, n-l2)+1;
      while(ng - ok > 1){
        int mid = (ok+ng)/2;
        if(is_same(l1,l1+mid,l2,l2+mid)) ok = mid;
        else ng = mid;
      }
      return ok;
    }
    int lcp(int l1, int l2, const RollingHash& y) const {
      assert(0 <= l1 && l1 <= n && 0 <= l2 && l2 <= y.size());
      int ok = 0; int ng = min(n-l1, y.size()-l2)+1;
      while(ng - ok > 1){
        int mid = (ok+ng)/2;
        if(is_same(l1,l1+mid,l2,l2+mid,y)) ok = mid;
        else ng = mid;
      }
      return ok;
    }
};

}  // namespace soryuusi
