// https://github.com/soryuusi0219/compro/blob/main/library/math/nimber.hpp
#pragma once
#include <array>
#include <bit>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <type_traits>

#if defined(__x86_64__) || defined(_M_X64)
#include <emmintrin.h>
#include <smmintrin.h>
#include <wmmintrin.h>
#pragma GCC target("pclmul")
#endif

namespace soryuusi {

using namespace std;

struct Nimber{
  using u64 = uint64_t;
  using u128 = __uint128_t;
  private:
    static constexpr array<u64,64> base = {0x0000000000000001ULL,0x19c9369f278adc02ULL,0xa181e7d66f5ff794ULL,0x447175c8e9f2810bULL,0x013b052fbd1cfb5cULL,0xf39b2f5fec45f0cbULL,0xa2bd0f68b29751c3ULL,0xa2d4d420d04d6d18ULL,0xf56a0fcdd6191e26ULL,0xacf45211cee311b4ULL,0x4b1e3b4118ceafe0ULL,0x164f73dc962955a3ULL,0xab9c67a0d6ce60b9ULL,0x0c94712fa5b4de4fULL,0x1cde1a1b8c56b44bULL,0xf979aa949989fc7aULL,0x4f6f64c7147fc9a2ULL,0xb1e83509fbd77f29ULL,0xba90f7de127a49c6ULL,0x1645a7444eacf2ffULL,0x0f6b020c1db6d459ULL,0x52fa3c2a968e1a9aULL,0xfaa0ddf94de5b9e3ULL,0xbf864438962f2d3dULL,0x04f95d621a3da30aULL,0x0dae6b5e975de827ULL,0x04e5ef0f58b0de80ULL,0xe34c5a5ac16239cdULL,0xa396d1188ff3ecceULL,0x4a2146b94df5d98bULL,0xe7cffb124206be8bULL,0xb08768f9d909b9ebULL,0xf2f92872b79c2ec2ULL,0x04d5a88d4e585236ULL,0xa2d6aef024069bb6ULL,0x0f05536fba17e1d9ULL,0x56c3f062e6587efdULL,0xf678e36dce663bb5ULL,0x4b0d758d19e71f5cULL,0xe0e253d290fe09a4ULL,0xbda1f4d4601beff2ULL,0x047eabf956cf8525ULL,0xe7798cfe1155eb85ULL,0x1e769066a1d10277ULL,0xe34a654b294cffe0ULL,0x1a59aa09f302fed6ULL,0xf38c27b926a233a9ULL,0xa5bec31d35c38d21ULL,0x06f9adf85ef52a0aULL,0x188cb91c34173a9cULL,0x57aacbc351e94437ULL,0xfb1c5212b6da1849ULL,0x002b044cd45c968bULL,0xa5ac24e50fc0814eULL,0x451aa8514c5c98cdULL,0x1d7f55695b986522ULL,0xfb5de61030204d3bULL,0x0198b56ae6ad29edULL,0xef142c75f096df50ULL,0x1af2e43c886831b2ULL,0xfcf1e0f268150229ULL,0xffa9e089e73261a0ULL,0xe327c09a856b791cULL,0x2d77a66ccc323348ULL};
    static constexpr array<u64,64> ibase = {0x0000000000000001ULL,0x4465cdfa46b09859ULL,0x61abc442bb5b4ff1ULL,0x4110d11bfacad7a0ULL,0x5211464d941718deULL,0x7d02399421168bc8ULL,0x6617d22f2582d06eULL,0x29523cd1a96e6748ULL,0x7c8bbe4110938955ULL,0x5c8bdb22528bc756ULL,0x413a421d43df0709ULL,0x1b0bc258f3d68661ULL,0x565802ec92b8780eULL,0x7c1272adbf5e0069ULL,0x38e7ed55697018c6ULL,0x5d037267f3950e33ULL,0x4068ec9fbcbb264aULL,0x6659c8956343ab5bULL,0x7493fff67d5ac0b9ULL,0x3abbd57f16b94810ULL,0x662d4dfdd9a34a06ULL,0x5473afd472cde5daULL,0x1772f5e555f85c73ULL,0x0f05a580cf1a39feULL,0x7a90b2a7c9cd33b8ULL,0x47dca8c55cdbcf07ULL,0x40a2294cd52de3a6ULL,0x0fa3ec6a806b84b7ULL,0x21cf869f832ea2e2ULL,0x3346486c33c72393ULL,0x75c0ed9970fcd4b0ULL,0x794397bf64ac7ffeULL,0x67d1639f50140ee3ULL,0x2b5dfe3598230aadULL,0x56374bedc64e67a4ULL,0x610015b31e4a5e53ULL,0x4d9d76c7be329735ULL,0x14a2ab0d7bb341cdULL,0x22fb1c5a6ea26306ULL,0x5e309df2ffc1dff4ULL,0x567acb09acc52479ULL,0x089b044ae0d78e4eULL,0x79eb709edaee3083ULL,0x15d630cc7a0d4f56ULL,0x1ca557589d2cbdc9ULL,0x0f8f1499c7a7e3beULL,0x095779ab0e6f33d9ULL,0x5afa751803ec8736ULL,0x454fe1cc83de858fULL,0x01a7cb24254c955fULL,0x6395bb8443b7886cULL,0x086e1e62945af7cfULL,0x6766b8093321f1f4ULL,0x48e588f09e066769ULL,0x09b893c74fef2910ULL,0x26659325e1d24769ULL,0x35cb57d78e038784ULL,0x345b0070a8c8ed2bULL,0x2e04bc3896c890b3ULL,0x21bc85580ef499abULL,0x4c60215e3fc40623ULL,0xa49623131b3891a6ULL,0x472c2db539baa70aULL,0xdc925994511e3754ULL};
    static constexpr auto make_block_table = [](const array<u64,64>& b){
      array<array<u64,256>,8> res{};
      for(int i=0; i<8; ++i) for(int j=0; j<8; ++j) for(int k=0; k<(1<<j); ++k) res[i][k|1<<j]=res[i][k]^b[i<<3|j];
      return res;
    };
    static constexpr auto conv = make_block_table(base);
    static constexpr auto iconv = make_block_table(ibase);

    static constexpr u64 N2P(u64 x){
      auto X = bit_cast<array<uint8_t,8>>(x);
      return conv[0][X[0]]^conv[1][X[1]]^conv[2][X[2]]^conv[3][X[3]]^conv[4][X[4]]^conv[5][X[5]]^conv[6][X[6]]^conv[7][X[7]];
    }
    static constexpr u64 P2N(u64 x){
      auto X = bit_cast<array<uint8_t,8>>(x);
      return iconv[0][X[0]]^iconv[1][X[1]]^iconv[2][X[2]]^iconv[3][X[3]]^iconv[4][X[4]]^iconv[5][X[5]]^iconv[6][X[6]]^iconv[7][X[7]];
    }
  public:
    u64 x;
    Nimber() : x(0){}
    Nimber(u64 x_) : x(N2P(x_)){};
    inline u64 get() const {return P2N(x);}
    friend Nimber operator+(Nimber a, Nimber b) { return a += b; }
    friend Nimber operator-(Nimber a, Nimber b) { return a -= b; }
    friend Nimber operator*(Nimber a, Nimber b) {
      u128 r = 0;
#if defined(__PCLMUL__) || (defined(_MSC_VER) && defined(__AVX__))
      if (!std::is_constant_evaluated()) {
        __m128i z = _mm_clmulepi64_si128(_mm_set_epi64x(0, a.x),_mm_set_epi64x(0, b.x),0x00);
        memcpy(&r,&z,16);
      }else
#endif
      {
        u64 t = a.x;
        u128 u = b.x;
        for(; t; t >>= 1, u <<= 1) if(t&1) r ^= u;
      }
      u64 h = r >> 64;
      h ^= (h >> 63) ^ (h >> 61) ^ (h >> 60);
      Nimber res;
      res.x = (h << 4) ^ (h << 3) ^ (h << 1) ^ h ^ r;
      return res;
    }
    friend Nimber operator/(Nimber a, Nimber b) { return a * b.inv();}
    friend Nimber& operator+=(Nimber& a, Nimber b) { a.x ^= b.x; return a;}
    friend Nimber& operator-=(Nimber& a, Nimber b) { a.x ^= b.x; return a;}
    friend Nimber& operator*=(Nimber& a, Nimber b) { return a = a * b;}
    friend Nimber& operator/=(Nimber& a, Nimber b) { return a = a / b;}
    Nimber operator-() const { return (*this); }
    Nimber inv() const { assert(x); return pow(-2);}
    Nimber pow(u64 b) const {
        Nimber a = *this, c = 1;
        while(b) {
          if(b & 1) c *= a;
          a *= a;
          b >>= 1;
        }
        return c;
    }
    friend bool operator==(Nimber a, Nimber b) { return a.x == b.x;}
    friend bool operator!=(Nimber a, Nimber b) { return a.x != b.x;}
    friend bool operator<(Nimber a, Nimber b) { return a.x < b.x;}
    friend bool operator>(Nimber a, Nimber b) { return a.x > b.x;}
    friend bool operator<=(Nimber a, Nimber b) { return a.x <= b.x;}
    friend bool operator>=(Nimber a, Nimber b) { return a.x >= b.x;}
    friend ostream& operator<<(ostream& os, const Nimber& a) { return os << a.get(); }
    friend istream& operator>>(istream& is, Nimber& a) {u64 tmp; is >> tmp; a = Nimber(tmp); return is;}
};

}  // namespace soryuusi
