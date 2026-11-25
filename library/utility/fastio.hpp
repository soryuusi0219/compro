// https://github.com/soryuusi0219/compro/blob/main/library/utility/fastio.hpp
#pragma once
#include <array>
#include <cassert>
#include <cstddef>
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <ostream>
#include <string>
#include <type_traits>
#include <utility>

#include "../math/internal/internal_bit.hpp"

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

    static bool digit(char c){return static_cast<unsigned>(c-'0') < 10;}

    static uint64_t digit_value(uint64_t word){
      word = (word * 10 + (word >> 8)) & 0xff00ff00ff00ffULL;
      word = (word * 100 + (word >> 16)) & 0xffff0000ffffULL;
      return ((word * 10000 + (word >> 32)) & 0xffffffffULL);
    }

    template<class UInt>
    UInt digits(){
      if(end-pos > 1 && !digit(buf[pos+1]) && digit(buf[pos])) return buf[pos++]-'0';
      UInt result = 0;
      do {
        if(end-pos < 8) refill();
        uint64_t word;
        memcpy(&word,buf+pos,8);
        if constexpr (soryuusi::endian::native == soryuusi::endian::big) word = soryuusi::byteswap(word);
        uint64_t values = word-0x3030303030303030ULL;
        // The high bit is set at the first byte below '0' or above '9'.
        uint64_t nondigits = (values | (word+0x4646464646464646ULL)) & 0x8080808080808080ULL;
        if(nondigits){
          int length = soryuusi::countr_zero(nondigits)/8;
          if(length == 0) return result;
          static constexpr uint32_t powers[] = {1,10,100,1000,10000,100000,1000000,10000000};
          result = result*powers[length]+digit_value(values << (64-length*8));
          pos += length;
          if(pos != end || eof) return result;
        }else{
          result = 100000000 * result + digit_value(values);
          pos += 8;
        }
      }while(true);
    }

    template<bool Signed>
    bool seek_integer(){
      for(;;){
        if(pos == end){
          refill();
          if(pos == end) return false;
        }
        char c = buf[pos];
        if(digit(c) || (Signed && c == '-')) return true;
        ++pos;
      }
    }

  public:
    Instream() = default;

    void skipSpace(){
      do {
        while(pos < end && buf[pos] != '\0' && buf[pos] <= ' ') ++pos;
        if(pos != end || eof) return;
        refill();
      }while(true);
    }

    template<typename UINT>
    UINT nextUint() {
      if(!seek_integer<false>()){
        assert(false && "integer expected");
        return 0;
      }
      return digits<UINT>();
    }

    template<typename INT>
    INT nextInt() {
      if(!seek_integer<true>()){
        assert(false && "integer expected");
        return 0;
      }
      const bool neg = buf[pos] == '-';
      pos += neg;
      using UINT = typename std::conditional_t<std::is_same_v<INT,__int128_t>,std::type_identity<__uint128_t>,std::make_unsigned<INT>>::type;
      UINT result = digits<UINT>();
      return neg ? static_cast<INT>(UINT(0) - result) : static_cast<INT>(result);
    }

    std::string nextToken(){
      skipSpace();
      std::string result;
      do {
        std::size_t start = pos;
        while(pos < end && buf[pos] > ' ') ++pos;
        result.append(buf+start,pos-start);
        if(pos != end || eof) return result;
        refill();
      }while(true);
    }

    char nextChar(){
      skipSpace();
      return pos != end ? buf[pos++] : '\0';
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
    inline void store_digits(u32 x, int count) {
      if constexpr (soryuusi::endian::native == soryuusi::endian::big) x = soryuusi::byteswap(x);
      memcpy(buf+pos,&x,4);
      pos += count;
    }
    inline void group(u32 x) {store_digits(LUT[x],4);}
    inline void first_group(u32 x) {
      unsigned int skip = 3 - (x >= 10) - (x >= 100) - (x >= 1000);
      u32 s = LUT[x] >> (skip * 8);
      store_digits(s,4-skip);
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
      if(pos) fwrite(buf, 1, pos, stdout);
      if(flush) fflush(stdout);
      pos = 0;
    }

  private:
    void put_u32(u32 x) {
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
    void put_u64(u64 x) {
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

    void put_fixed19(u64 x) {
      u64 q1 = div_1e4(x), q2 = div_1e8(x), q3 = div_1e12(x), q4 = div_1e16(x);
      u32 s = LUT[q4] >> 8;
      store_digits(s,3);
      group(q3 - q4 * 10000);
      group(q2 - q3 * 10000);
      group(q1 - q2 * 10000);
      group(x - q1 * 10000);
    }

    void put_u128(u128 x) {
      constexpr u128 limit = u128(10'000'000'000'000'000'000ULL)*10'000'000'000'000'000'000ULL;
      int high = 0;
      for(;x >= limit;++high) x -= limit;
      auto [q,r] = divmod_1e19(x);
      if(high){
        buf[pos++] = '0'+high;
        put_fixed19(q);
      }else if(q) put_u64(q);
      if(high || q) put_fixed19(r);
      else put_u64(r);
    }

  public:
    void nextU32(u32 x) {reserve(10); put_u32(x);}
    void nextU64(u64 x) {reserve(20); put_u64(x);}
    void fixed19(u64 x) {reserve(19); put_fixed19(x);}
    void nextU128(u128 x) {reserve(39); put_u128(x);}

    void nextI32(i32 x){
      reserve(11);
      if(x < 0) buf[pos++] = '-', put_u32(u32(0)-static_cast<u32>(x));
      else put_u32(static_cast<u32>(x));
    }
    void nextI64(i64 x){
      reserve(20);
      if(x < 0) buf[pos++] = '-', put_u64(u64(0)-static_cast<u64>(x));
      else put_u64(static_cast<u64>(x));
    }
    void nextI128(i128 x){
      reserve(40);
      if(x < 0) buf[pos++] = '-', put_u128(u128(0)-static_cast<u128>(x));
      else put_u128(static_cast<u128>(x));
    }

    void nextCstr(const char* x){
      std::size_t length = strlen(x);
      if(length > BUF_SIZE){
        write();
        fwrite(x,1,length,stdout);
        return;
      }
      reserve(length);
      memcpy(buf+pos,x,length);
      pos += length;
    }
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

    Outstream& operator<<(std::ostream& (*pf)(std::ostream&)) {
      using OstreamManip = std::ostream& (*)(std::ostream&);
      constexpr OstreamManip endl_ptr  = std::endl;
      constexpr OstreamManip ends_ptr  = std::ends;
      constexpr OstreamManip flush_ptr = std::flush;
      if (pf == endl_ptr) {nextChar('\n'); write(1); return *this; }
      if (pf == ends_ptr) {nextChar('\0'); return *this; }
      if (pf == flush_ptr) {write(true); return *this; }
      return *this;
    }
};

inline Instream<> cin;
inline Outstream<> cout;

} // namespace soryuusi
