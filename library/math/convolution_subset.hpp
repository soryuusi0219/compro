// https://github.com/soryuusi0219/compro/blob/main/library/math/convolution_subset.hpp
#pragma once
#include <algorithm>
#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <span>
#include <vector>

#include "internal/internal_bit.hpp"
#include "internal/modint_traits.hpp"

namespace soryuusi {

namespace internal {

// One ranked zeta transform can be reused when the left operand is fixed.
template<class T>
class SubsetConvolution {
  static constexpr std::size_t direct_limit = 64;
  static constexpr bool small_modint = [] {
    if constexpr (is_modint_like_v<T>) return sizeof(decltype(T().val())) <= 4;
    else return false;
  }();

  std::size_t size;
  int dim, narrow_terms = 0;
  std::vector<std::uint8_t> rank;
  std::vector<std::size_t> offset;
  std::span<const T> left;
  std::vector<T> fixed, work;

  template<class UInt>
  static T reduce(UInt value){
    return T::raw(value % T::mod());
  }

  template<class UInt, bool Square>
  static T dot(const T* a, const T* b, int first, int last, int degree){
    UInt sum = 0;
    if constexpr (Square){
      for(int i = first; i < (degree+1)/2; ++i) sum += std::uint64_t(a[i].val())*a[degree-i].val();
      sum += sum;
      if(degree%2 == 0) sum += std::uint64_t(a[degree/2].val())*a[degree/2].val();
    }else{
      for(int i = first; i <= last; ++i) sum += std::uint64_t(a[i].val())*b[degree-i].val();
    }
    return reduce(sum);
  }

  template<class UInt = std::uint64_t>
  void direct(std::span<const T> a, std::span<const T> b, std::span<T> out){
    work.resize(size);
    for(std::size_t mask = 0; mask < size; ++mask){
      if constexpr (small_modint){
        UInt sum = 0;
        for(std::size_t i = mask;; i = (i-1)&mask){
          if(i < a.size() && (mask^i) < b.size()) sum += std::uint64_t(a[i].val())*b[mask^i].val();
          if(i == 0) break;
        }
        work[mask] = reduce(sum);
      }else{
        T sum = 0;
        for(std::size_t i = mask;; i = (i-1)&mask){
          if(i < a.size() && (mask^i) < b.size()) sum += a[i]*b[mask^i];
          if(i == 0) break;
        }
        work[mask] = sum;
      }
    }
    std::copy(work.begin(),work.end(),out.begin());
  }

  void multiply_direct(std::span<const T> a, std::span<const T> b, std::span<T> out){
    if constexpr (small_modint){
      if(std::min(a.size(),b.size()) <= std::size_t(narrow_terms)) direct<std::uint64_t>(a,b,out);
      else direct<__uint128_t>(a,b,out);
    }else direct(a,b,out);
  }

  template<class Block>
  void butterflies(Block apply) const {
    // Run the low-bit passes while their rows still fit in cache.
    constexpr std::size_t cache_bytes = 128*1024;
    const auto chunk = std::min(size,soryuusi::bit_floor(std::max<std::size_t>(1,cache_bytes/((dim+1)*sizeof(T)))));
    for(std::size_t first = 0; first < size; first += chunk)
      for(std::size_t bit = 1; bit < chunk; bit <<= 1)
        for(std::size_t block = first; block < first+chunk; block += 2*bit) apply(block,bit);
    for(std::size_t bit = chunk; bit < size; bit <<= 1)
      for(std::size_t block = 0; block < size; block += 2*bit) apply(block,bit);
  }

  void zeta(std::span<const T> a, std::vector<T>& table) const {
    table.assign(offset.back(),T(0));
    for(std::size_t mask = 0; mask < a.size(); ++mask) table[offset[mask]+rank[mask]] = a[mask];
    butterflies([&](std::size_t block, std::size_t bit){
      // Larger bits are still fixed. Only already processed bits may disappear.
      const int first = rank[block];
      T* low = table.data()+offset[block];
      T* high = table.data()+offset[block+bit];
      for(std::size_t i = 0; i < bit; ++i){
        const int last = rank[i];
        for(int r = first; r <= first+last; ++r) high[r] += low[r];
        low += first+last+1;
        high += first+last+2;
      }
    });
  }

  template<bool Square = false>
  void multiply_ranks(const std::vector<T>& a, std::vector<T>& b) const {
    std::array<T,std::numeric_limits<std::size_t>::digits> product;
    const T* x = a.data();
    T* y = b.data();
    for(std::size_t mask = 0; mask < size; ++mask){
      const int p = rank[mask], last = std::min(dim,2*p);
      // Input rows cover [0,p]. Only degrees [p,min(dim,2p)] survive inversion.
      for(int degree = p; degree <= last; ++degree){
        if constexpr (small_modint){
          if(2*p-degree+1 <= narrow_terms) product[degree-p] = dot<std::uint64_t,Square>(x,y,degree-p,p,degree);
          else product[degree-p] = dot<__uint128_t,Square>(x,y,degree-p,p,degree);
        }else{
          T sum = 0;
          for(int i = degree-p; i <= p; ++i) sum += x[i]*y[degree-i];
          product[degree-p] = sum;
        }
      }
      std::copy_n(product.begin(),last-p+1,y);
      x += p+1;
      y += p+1;
    }
  }

  void recover(std::vector<T>& product, std::span<T> out) const {
    butterflies([&](std::size_t block, std::size_t bit){
      const int base = rank[block];
      const T* low = product.data()+offset[block];
      T* high = product.data()+offset[block+bit];
      for(std::size_t i = 0; i < bit; ++i){
        const int p = base+rank[i];
        // At rank p+1, degree p+1 is at offset 0 instead of offset 1.
        for(int r = 0; r < std::min(dim-p,p); ++r) high[r] -= low[r+1];
        low += p+1;
        high += p+2;
      }
    });
    for(std::size_t mask = 0; mask < size; ++mask) out[mask] = product[offset[mask]];
  }

public:
  explicit SubsetConvolution(std::size_t n) : size(n), dim(soryuusi::countr_zero(n)){
    assert(soryuusi::has_single_bit(n));
    if constexpr (small_modint){
      const std::uint64_t bound = T::mod()-1;
      narrow_terms = bound == 0 ? direct_limit :
        std::min<std::uint64_t>(direct_limit,std::numeric_limits<std::uint64_t>::max()/bound/bound);
    }
    if(size > direct_limit){
      rank.resize(size);
      offset.resize(size+1);
      for(std::size_t i = 1; i < size; ++i) rank[i] = rank[i>>1]+(i&1);
      for(std::size_t i = 0; i < size; ++i) offset[i+1] = offset[i]+rank[i]+1;
    }
  }

  void set_left(std::span<const T> a){
    assert(a.size() <= size);
    // Small products borrow the operand until the last multiply call.
    if(size <= direct_limit) left = a;
    else zeta(a,fixed);
  }

  void multiply(std::span<const T> b, std::span<T> out){
    assert(b.size() <= size && out.size() == size);
    if(size <= direct_limit) multiply_direct(left,b,out);
    else {
      zeta(b,work);
      multiply_ranks(fixed,work);
      recover(work,out);
    }
  }

  void square(std::span<const T> a, std::span<T> out){
    assert(a.size() <= size && out.size() == size);
    if(size <= direct_limit) multiply_direct(a,a,out);
    else {
      zeta(a,work);
      multiply_ranks<true>(work,work);
      recover(work,out);
    }
  }
};

}  // namespace internal

template<class T>
std::vector<T> convolution_subset(const std::vector<T>& a, const std::vector<T>& b){
  if(a.empty() || b.empty()) return {};
  const auto size = soryuusi::bit_ceil(std::max(a.size(),b.size()));
  std::vector<T> result(size);
  if(a.size() == 1){
    std::transform(b.begin(),b.end(),result.begin(),[&](const T& value){return a[0]*value;});
    return result;
  }
  if(b.size() == 1){
    std::transform(a.begin(),a.end(),result.begin(),[&](const T& value){return value*b[0];});
    return result;
  }
  internal::SubsetConvolution<T> plan(size);
  if(&a == &b) plan.square(a,result);
  else {
    plan.set_left(a);
    plan.multiply(b,result);
  }
  return result;
}

}  // namespace soryuusi
