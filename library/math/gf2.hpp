// https://github.com/soryuusi0219/compro/blob/main/library/math/gf2.hpp
#pragma once
#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <iostream>

namespace soryuusi {

using namespace std;

template<int D>
struct GF2{
  using u64 = uint64_t;
  static_assert(0 < D && D < 64);
  static constexpr u64 msks[] =  {0x1,0x3,0x7,0xb,0x13,0x25,0x43,0x83,0x11b,0x203,0x409,0x805,0x1009,0x201b,0x4021,0x8003,0x1002b,0x20009,0x40009,0x80027,0x100009,0x200005,0x400003,0x800021,0x100001b,0x2000009,0x400001b,0x8000027,0x10000003,0x20000005,0x40000003,0x80000009,0x10000008d,0x20000004b,0x40000001b,0x800000005,0x1000000035,0x200000003f,0x4000000063,0x8000000011,0x10000000039,0x20000000009,0x40000000027,0x80000000059,0x100000000021,0x20000000001b,0x400000000003,0x800000000021,0x100000000002d,0x2000000000071,0x400000000001d,0x800000000004b,0x10000000000009,0x20000000000047,0x4000000000007d,0x80000000000047,0x100000000000095,0x200000000000011,0x400000000000063,0x80000000000007b,0x1000000000000003,0x2000000000000027,0x4000000000000069,0x8000000000000003};
  static constexpr u64 msk = msks[D];
  static constexpr u64 mul(u64 x, u64 y){
    u64 res = 0;
    while(y){
      if(y&1) res ^= x;
      x <<= 1;
      x = min(x,x^msk);
      y >>= 1;
    }
    return res;
  }

  u64 x;
  GF2(u64 x_ = 0) : x(x_) {}
  friend GF2 operator+(GF2 a, GF2 b) { return a.x ^ b.x; }
  friend GF2 operator-(GF2 a, GF2 b) { return a.x ^ b.x; }
  friend GF2 operator*(GF2 a, GF2 b) { return mul(a.x, b.x); }
  friend GF2 operator/(GF2 a, GF2 b) { return mul(a.x, b.inv().x); }
  friend GF2& operator+=(GF2& a, GF2 b) { return a = a.x ^ b.x; }
  friend GF2& operator-=(GF2& a, GF2 b) { return a = a.x ^ b.x; }
  friend GF2& operator*=(GF2& a, GF2 b) { return a = mul(a.x,b.x); }
  friend GF2& operator/=(GF2& a, GF2 b) { return a = mul(a.x, b.inv()); }
  friend bool operator==(const GF2& a, const GF2& b) { return a.x == b.x; }
  friend bool operator!=(const GF2& a, const GF2& b) { return a.x != b.x; }
  friend bool operator<(const GF2& a, const GF2& b) { return a.x < b.x; }
  friend bool operator>(const GF2& a, const GF2& b) { return a.x > b.x; }
  friend bool operator<=(const GF2& a, const GF2& b) { return a.x <= b.x; }
  friend bool operator>=(const GF2& a, const GF2& b) { return a.x >= b.x; }

  GF2 operator-() const { return (*this); }

  friend ostream& operator<<(ostream& os, const GF2& a) { return os << a.x; }
  friend istream& operator>>(istream& is, GF2& a) { return is >> a.x; }

  friend GF2& operator<<=(GF2& a, u64 k) {
    while(k--) {
      a.x <<= 1;
      a.x = min(a.x,a.x^msk);
    }
    return a;
  }
  friend GF2 operator<<(GF2 a, u64 k) {
    return a <<= k;
  }
  
  GF2 inv() const { assert(x); return pow((1ll << D) - 2); }
  GF2 pow(u64 b) const {
    GF2 a = *this, c = 1;
    while(b) {
      if(b & 1) c *= a;
      a *= a;
      b >>= 1;
    }
    return c;
  }
  GF2 sqrt() const {
    GF2 res = *this;
    for(int i = D; --i;) res *= res;
    return res;
  }
};

}  // namespace soryuusi
