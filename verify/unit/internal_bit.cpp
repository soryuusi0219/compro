#include <array>
#include <cassert>
#include <cstdint>
#include <limits>
#include <random>

#include "library/math/internal/internal_bit.hpp"

namespace {

template<class U>
void check_counts(U x){
  const int digits = std::numeric_limits<U>::digits;
  int width = 0, trailing = digits, ones = 0;
  for(int i = 0; i < digits; ++i){
    if((x >> i) & 1){
      width = i+1;
      if(!ones) trailing = i;
      ++ones;
    }
  }
  assert(soryuusi::bit_width(x) == width);
  assert(soryuusi::countl_zero(x) == digits-width);
  assert(soryuusi::countr_zero(x) == trailing);
  assert(soryuusi::popcount(x) == ones);
  assert(soryuusi::has_single_bit(x) == (ones == 1));
  assert(soryuusi::bit_floor(x) == (width ? static_cast<U>(U{1}<<(width-1)) : U{0}));
  if(x <= (U{1}<<(digits-1))){
    int ceil_shift = width ? width-(ones==1) : 0;
    assert(soryuusi::bit_ceil(x) == static_cast<U>(U{1}<<ceil_shift));
  }
  assert(soryuusi::countr_one(x) == soryuusi::countr_zero(static_cast<U>(~x)));
  assert(soryuusi::countl_one(x) == soryuusi::countl_zero(static_cast<U>(~x)));
  assert(soryuusi::rotr(soryuusi::rotl(x,3),3) == x);
}

}  // namespace

int main(){
  static_assert(soryuusi::bit_width(0U) == 0,"empty mask");
  static_assert(soryuusi::countr_zero(std::uint64_t{0}) == 64,"64-bit zero");
  static_assert(soryuusi::bit_ceil(std::uint32_t{17}) == 32,"next power of two");
  for(int value = 0; value < 256; ++value) check_counts(static_cast<std::uint8_t>(value));
  check_counts(std::uint16_t{0});
  check_counts(std::uint32_t{0});
  check_counts(std::uint64_t{1}<<63);
  std::mt19937_64 rng(36119);
  for(int i = 0; i < 1000; ++i){
    const std::uint64_t x = rng();
    check_counts(static_cast<std::uint16_t>(x));
    check_counts(static_cast<std::uint32_t>(x));
    check_counts(x);
  }
  const std::uint32_t value = 0x12345678U;
  const auto bytes = soryuusi::bit_cast<std::array<unsigned char,sizeof(value)>>(value);
  assert(soryuusi::bit_cast<std::uint32_t>(bytes) == value);
}
