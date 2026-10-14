// https://github.com/soryuusi0219/compro/blob/main/library/utility/fastio.hpp
#pragma once
#include <array>
#include <cassert>
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <ostream>
#include <string>
#include <type_traits>
#include <utility>

namespace soryuusi{

using namespace std;

template<unsigned int BUF_SIZE = (1U << 16)>
class Instream {
    static_assert(BUF_SIZE >= 64);
    // The extra bytes are zero sentinels for the eight-digit parser.
    char buf[BUF_SIZE + 8]{};
    std::size_t pos = 0, end = 0;
    bool eof = false;

    void refill() {
      if (eof) return;
      const std::size_t remain = end - pos;
      if (remain) std::memmove(buf, buf + pos, remain);
      pos = 0;
      end = remain;
      const std::size_t read = std::fread(buf + end, 1, BUF_SIZE - end, stdin);
      end += read;
      eof = (read == 0);
      std::memset(buf + end, 0, 8);
    }

    void ensure(std::size_t n) {
      while (end - pos < n && !eof) refill();
    }

    char peek() {
      ensure(1);
      return pos == end ? '\0' : buf[pos];
    }

  public:
    Instream() { refill(); }

    void skipSpace(){ while (peek() != '\0' && peek() <= ' ') ++pos; }
    
    template<typename UINT>
    UINT nextUint() {
      while (peek() < '0' || peek() > '9') {
        assert(peek() != '\0');
        ++pos;
      }
      UINT res = 0;
      uint64_t tmp = 0;
      while(1) {
        ensure(8);
        memcpy(&tmp, buf + pos, 8);
        if ((tmp -= 0x3030303030303030) & 0x8080808080808080) break;
        tmp = (tmp * 10 + (tmp >> 8)) & 0xff00ff00ff00ff;
        tmp = (tmp * 100 + (tmp >> 16)) & 0xffff0000ffff;
        tmp = (tmp * 10000 + (tmp >> 32)) & 0xffffffff;
        res = 100000000 * res + tmp;
        pos += 8;
      }
      while ('0' <= peek() && peek() <= '9') res = res * 10 + (buf[pos++] - '0');
      return res;
    }
    template<typename INT>
    INT nextInt() {
      while ((peek() < '0' || peek() > '9') && peek() != '-') {
        assert(peek() != '\0');
        ++pos;
      }
      const bool neg = peek() == '-';
      if (neg) ++pos;
      using UINT = std::make_unsigned_t<INT>;
      UINT res = 0;
      uint64_t tmp = 0;
      while(1) {
        ensure(8);
        memcpy(&tmp, buf + pos, 8);
        if ((tmp -= 0x3030303030303030) & 0x8080808080808080) break;
        tmp = (tmp * 10 + (tmp >> 8)) & 0xff00ff00ff00ff;
        tmp = (tmp * 100 + (tmp >> 16)) & 0xffff0000ffff;
        tmp = (tmp * 10000 + (tmp >> 32)) & 0xffffffff;
        res = 100000000 * res + tmp;
        pos += 8;
      }
      while ('0' <= peek() && peek() <= '9') res = res * 10 + (buf[pos++] - '0');
      return neg ? static_cast<INT>(UINT(0) - res) : static_cast<INT>(res);
    }
    std::string nextToken(){
      skipSpace();
      std::string result;
      while (peek() > ' ') {
        result.push_back(buf[pos++]);
      }
      return result;
    }
    char nextChar(){
      skipSpace();
      return buf[pos++];
    }

    Instream& operator>>(unsigned int& dest){ dest = nextUint<uint32_t>(); return *this; }
    Instream& operator>>(int& dest){ dest = nextInt<int32_t>(); return *this; }
    Instream& operator>>(unsigned long& dest){ dest = nextUint<uint64_t>(); return *this; }
    Instream& operator>>(long& dest){ dest = nextInt<int64_t>(); return *this; }
    Instream& operator>>(unsigned long long& dest){ dest = nextUint<uint64_t>(); return *this; }
    Instream& operator>>(long long& dest){ dest = nextInt<int64_t>(); return *this; }
    Instream& operator>>(__uint128_t& dest){ dest = nextUint<__uint128_t>(); return *this; }
    Instream& operator>>(__int128_t& dest){ dest = nextInt<__int128_t>(); return *this; }
    Instream& operator>>(std::string& dest){ dest = nextToken(); return *this; }
    Instream& operator>>(char& dest){ dest = nextChar(); return *this; }

    template <class Mint>
      requires (!std::is_integral_v<Mint> && requires(Mint value) {
        Mint::mod();
        value.val();
        Mint(0);
      })
    Instream& operator>>(Mint& dest) {
      skipSpace();
      const bool negative = peek() == '-';
      if (negative || peek() == '+') ++pos;
      assert('0' <= peek() && peek() <= '9');
      Mint value = 0;
      while ('0' <= peek() && peek() <= '9') {
        value = value * 10 + (buf[pos++] - '0');
      }
      dest = negative ? -value : value;
      return *this;
    }
};

template<unsigned int BUF_SIZE = (1U << 16)>
class Outstream {
    using u32 = uint32_t;
    using u64 = uint64_t;
    using u128 = __uint128_t;
    using i32 = int32_t;
    using i64 = int64_t;
    using i128 = __int128_t;

    static_assert(BUF_SIZE >= 64);
    char buf[BUF_SIZE];
    std::size_t pos = 0;

    static constexpr array<u32, 10000> LUT = []{
      array<u32, 10000> result{};
      for (int x = 0; x < 10000; ++x) result[x] = (u32)('0' + x / 1000) | (u32)('0' + x / 100 % 10) << 8 | (u32)('0' + x / 10 % 10) << 16 | (u32)('0' + x % 10) << 24;
      return result;
    }();

    inline void reserve(std::size_t n) { if (BUF_SIZE - pos < n) write(); }
    inline void group(u32 x) {reserve(4); memcpy(buf + pos, &LUT[x], 4); pos += 4;}
    inline void first_group(u32 x) {
      unsigned int skip = 3 - (x >= 10) - (x >= 100) - (x >= 1000);
      u32 s = LUT[x] >> (skip * 8);
      reserve(4);
      memcpy(buf + pos, &s, 4);
      pos += 4 - skip;
    }
    
    inline static u32 div_1e4(u32 x) {return ((u64)x * 0xd1b71759ULL) >> 45;}
    inline static u32 div_1e8(u32 x) {return ((u64)x * 0xabcc7712ULL) >> 58;}

    inline static u64 div_1e4(u64 x) {return ((u128)x * 0x346dc5d63886594bULL) >> 75;}
    inline static u64 div_1e8(u64 x) {return ((u128)x * 0xabcc77118461cefdULL) >> 90;}
    inline static u64 div_1e12(u64 x) {return ((u128)x * 0x232f33025bd42233ULL) >> 101;}
    inline static u64 div_1e16(u64 x) {return ((u128)x * 0x39a5652fb1137857ULL) >> 115;}

    inline u128 mulhi(u128 a, u128 b) {
        u64 al = (u64)a, ah = (u64)(a >> 64);
        u64 bl = (u64)b, bh = (u64)(b >> 64);
        u128 ll = (u128)al * bl, lh = (u128)al * bh;
        u128 hl = (u128)ah * bl, hh = (u128)ah * bh;
        u128 mid = (ll >> 64) + (u64)lh + (u64)hl;
        return hh + (lh >> 64) + (hl >> 64) + (mid >> 64);
    }
    inline pair<u64, u64> divmod_1e19(u128 x) {
        constexpr u64 B = 10'000'000'000'000'000'000ULL;
        constexpr u128 M = ((u128)0xd83c94fb6d2ac34aULL << 64) | 0x5663d3c7a0d865cbULL;
        if (x < B) return {0, (u64)x};
        u128 h = mulhi(x, M), s = x + h;
        u64 q = (u64)((s >> 64) + (s < x));
        return {q, (u64)(x - (u128)q * B)};
    }

  public:
    Outstream() = default;
    ~Outstream() {
      write();
    }

    void write(bool flush = false){
      fwrite(buf, 1, pos, stdout);
      if(flush) fflush(stdout);
      pos = 0;
    }
    
    void nextU32(u32 x) {
      if (x > 9999'9999U) {
        u64 q1 = div_1e4(x), q2 = div_1e8(x);
        first_group(q2);
        group(q1 - q2 * 10000);
        group(x - q1 * 10000);
      }else if (x > 9999U) {
        u64 q1 = div_1e4(x);
        first_group(q1);
        group(x - q1 * 10000);
      }else{
        first_group(x);
      }
    }
    void nextU64(u64 x) {
      if (x > 9999'9999'9999'9999ULL) {
        u64 q1 = div_1e4(x), q2 = div_1e8(x), q3 = div_1e12(x), q4 = div_1e16(x);
        first_group(q4);
        group(q3 - q4 * 10000);
        group(q2 - q3 * 10000);
        group(q1 - q2 * 10000);
        group(x - q1 * 10000);
      }else if (x > 9999'9999'9999ULL) {
        u64 q1 = div_1e4(x), q2 = div_1e8(x), q3 = div_1e12(x);
        first_group(q3);
        group(q2 - q3 * 10000);
        group(q1 - q2 * 10000);
        group(x - q1 * 10000);
      }else if (x > 9999'9999ULL) {
        u64 q1 = div_1e4(x), q2 = div_1e8(x);
        first_group(q2);
        group(q1 - q2 * 10000);
        group(x - q1 * 10000);
      }else if (x > 9999ULL) {
        u64 q1 = div_1e4(x);
        first_group(q1);
        group(x - q1 * 10000);
      }else{
        first_group(x);
      }
    }

    void fixed19(u64 x) {
      u64 q1 = div_1e4(x), q2 = div_1e8(x), q3 = div_1e12(x), q4 = div_1e16(x);
      u32 s = LUT[q4] >> 8;
      reserve(4); memcpy(buf + pos, &s, 4); pos += 3;
      group(q3 - q4 * 10000);
      group(q2 - q3 * 10000);
      group(q1 - q2 * 10000);
      group(x - q1 * 10000);
    }

    void nextI32(i32 x){
      if(x < 0) nextChar('-'), nextU32(u32(0) - static_cast<u32>(x));
      else nextU32(static_cast<u32>(x));
    }
    void nextI64(i64 x) {
      if(x < 0) nextChar('-'), nextU64(u64(0) - static_cast<u64>(x));
      else nextU64(static_cast<u64>(x));
    }

    void nextU128(u128 x) {
      static constexpr u128 T3e38 = (u128)10'000'000'000'000'000'000ULL * 10'000'000'000'000'000'000ULL * 3;
      static constexpr u128 T2e38 = (u128)10'000'000'000'000'000'000ULL * 10'000'000'000'000'000'000ULL * 2;
      static constexpr u128 T1e38 = (u128)10'000'000'000'000'000'000ULL * 10'000'000'000'000'000'000ULL;
      if(x >= T3e38) x -= T3e38, nextChar('3');
      else if(x >= T2e38) x -= T2e38, nextChar('2');
      else if(x >= T1e38) x -= T1e38, nextChar('1');
      auto [q, r] = divmod_1e19(x);
      if(q) nextU64(q), fixed19(r);
      else nextU64(r);
    }

    void nextI128(i128 x) {
      bool neg = x < 0;
      u128 v = neg ? u128(0) - (u128)x : (u128)x;
      auto [q, r] = divmod_1e19(v);
      if(neg){
        nextChar('-');
        if(q) nextU64(q), fixed19(r);
        else nextU64(r);
      }else{
        if(q) nextU64(q), fixed19(r);
        else nextU64(r);
      }
    }

    void nextCstr(const char* x){ while(*x) nextChar(*(x++)); }
    void nextChar(char x){reserve(1); buf[pos++] = x; }
    Outstream& operator<<(unsigned int targ){ nextU32(targ); return *this; }
    Outstream& operator<<(int targ){ nextI32(targ); return *this; }
    Outstream& operator<<(unsigned long targ){ nextU64(targ); return *this; }
    Outstream& operator<<(long targ){ nextI64(targ); return *this; }
    Outstream& operator<<(unsigned long long targ){ nextU64(targ); return *this; }
    Outstream& operator<<(long long targ){ nextI64(targ); return *this; }
    Outstream& operator<<(__uint128_t targ){ nextU128(targ); return *this; }
    Outstream& operator<<(__int128_t targ){ nextI128(targ); return *this; }
    Outstream& operator<<(const string& targ){ nextCstr(targ.c_str()); return *this; }
    Outstream& operator<<(const char* targ){ nextCstr(targ); return *this; }
    Outstream& operator<<(char targ){ nextChar(targ); return *this; }

    template <class Mint>
      requires (!std::is_integral_v<Mint> && requires(const Mint& value) {
        Mint::mod();
        value.val();
      })
    Outstream& operator<<(const Mint& value) {
      return *this << value.val();
    }
    Outstream& operator<<(std::ostream& (*pf)(std::ostream&)) {
      using OstreamManip = std::ostream& (*)(std::ostream&);
      constexpr OstreamManip endl_ptr  = std::endl;
      constexpr OstreamManip ends_ptr  = std::ends;
      constexpr OstreamManip flush_ptr = std::flush;
      if (pf == endl_ptr) {nextChar('\n'); write(1); return *this; }
      if (pf == ends_ptr) {nextChar('\0'); return *this; }
      if (pf == flush_ptr) {write(); return *this; }
      return *this;
    }
};

inline Instream<> cin;
inline Outstream<> cout;

} // namespace soryuusi
