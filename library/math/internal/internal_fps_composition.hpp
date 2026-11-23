// https://github.com/soryuusi0219/compro/blob/main/library/math/internal/internal_fps_composition.hpp
#pragma once
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <tuple>
#include <variant>
#include <utility>
#include <vector>

#include "internal_bit.hpp"
#include "../convolution.hpp"
#include "../convolution_mod.hpp"
#include "modint_traits.hpp"
#include "../ntt.hpp"

namespace soryuusi::internal {

// Large 64-bit target moduli can exceed the exact lift of the three CRT primes.
template<class T>
bool fps_crt_fits(std::size_t terms){
  constexpr __uint128_t capacity = __uint128_t(167772161)*469762049*754974721;
  const std::uint64_t bound = T::mod()-1;
  return terms <= 60 || bound == 0 || terms <= (capacity-1)/bound/bound;
}

template<class T>
std::vector<T> fps_product(const std::vector<T>& a, const std::vector<T>& b){
  if constexpr (is_ntt_friendly_v<T>){
    if(&a == &b) return soryuusi::convolution_square(a);
    return soryuusi::convolution(a,b);
  }
  else{
    if(fps_crt_fits<T>(std::min(a.size(),b.size()))) return soryuusi::convolution_mod(a,b);
    std::vector<T> c(a.size()+b.size()-1);
    for(std::size_t i = 0; i < a.size(); ++i)
      for(std::size_t j = 0; j < b.size(); ++j) c[i+j] += a[i]*b[j];
    return c;
  }
}

template<class T>
std::vector<T> fps_middle_product(std::vector<T> a, std::vector<T> b){
  if constexpr (is_ntt_friendly_v<T>) return soryuusi::middle_product(std::move(a),std::move(b));
  else{
    if(a.size() < b.size() || b.empty()) return {};
    if(fps_crt_fits<T>(b.size())) return convolution_mod_mint<true>(a,b);
    auto product = fps_product(a,b);
    product.resize(a.size());
    product.erase(product.begin(),product.begin()+(b.size()-1));
    return product;
  }
}

// With h = width/2, store the x-parities as polynomials in x^2 and y.
// Each y row starts width entries apart, so its x products cannot carry.
template<class T>
struct FPSParityKernel {
  int width;
  std::vector<T> even, odd;

  FPSParityKernel(const std::vector<T>& q, int w) : width(w) {
    std::tie(even,odd) = parity(q,w);
  }

  static std::pair<std::vector<T>,std::vector<T>> parity(const std::vector<T>& q, int width){
    const int rows = q.size()/width, h = width/2;
    std::vector<T> even(q.size()-h), odd(q.size()-h);
    for(int row = 0; row < rows; ++row){
      const int offset = row*width;
      for(int i = 0; i < h; ++i){
        even[offset+i] = q[offset+2*i];
        odd[offset+i] = q[offset+2*i+1];
      }
    }
    return {std::move(even),std::move(odd)};
  }

  std::vector<T> next_denominator(int rows) const {
    auto a = fps_product(even,even), b = fps_product(odd,odd);
    const int h = width/2;
    std::vector<T> q(rows*h);
    for(int row = 0; row < rows; ++row){
      const int offset = row*width;
      q[row*h] = a[offset];
      for(int i = 1; i < h; ++i) q[row*h+i] = a[offset+i]-b[offset+i-1];
    }
    return q;
  }

  std::vector<T> next_numerator(const std::vector<T>& p, int rows) const {
    auto [pe,po] = parity(p,width);
    auto a = fps_product(po,even), b = fps_product(pe,odd);
    const int h = width/2;
    std::vector<T> result(rows*h);
    for(int row = 0; row < rows; ++row)
      for(int i = 0; i < h; ++i) result[row*h+i] = a[row*width+i]-b[row*width+i];
    return result;
  }

  // Transpose odd(P*Q(-x)) using the middle product of each parity.
  std::vector<T> transpose(std::vector<T> weights) const {
    const int h = width/2, rows = weights.size()/width;
    const std::size_t left_size = weights.size()-h;
    std::vector<T> encoded(left_size+even.size()-1);
    for(std::size_t row = 0; row < weights.size()/h; ++row)
      std::copy_n(weights.begin()+row*h,h,encoded.begin()+row*width);
    auto e = fps_middle_product(encoded,std::vector<T>(odd.rbegin(),odd.rend()));
    auto o = fps_middle_product(std::move(encoded),std::vector<T>(even.rbegin(),even.rend()));
    for(int row = 0; row < rows; ++row){
      const int offset = row*width;
      for(int i = 0; i < h; ++i){
        weights[offset+2*i] = -e[offset+i];
        weights[offset+2*i+1] = o[offset+i];
      }
    }
    return weights;
  }
};

template<class FPS>
class FPSCompositionCRT {
  using T = typename FPS::value_type;
  using Kernel = FPSParityKernel<T>;
  int precision = 0, width = 0;
  bool zero_constant = true;
  std::vector<Kernel> kernels;
  FPS denominator;

  static std::vector<T> initial_denominator(const FPS& g, int w, int rows){
    std::vector<T> q(w*rows);
    q[0] = 1;
    if(rows == 2){
      for(int i = 0; i < std::min<int>(w,g.size()); ++i) q[w+i] = -g[i];
    }
    return q;
  }

public:
  void init(const FPS& g, int m){
    precision = m;
    if(m == 0) return;
    width = soryuusi::bit_ceil(static_cast<unsigned int>(m));
    zero_constant = g.empty() || g[0] == T(0);
    auto q = initial_denominator(g,width,2);
    int rows = 2;
    for(int w = width; w > 1; w /= 2){
      kernels.emplace_back(q,w);
      rows = 2*rows-1;
      q = kernels.back().next_denominator(rows);
    }
    if(!zero_constant) denominator = FPS(std::move(q)), denominator.shrink();
  }

  FPS composition(FPS f) const {
    if(precision == 0) return {};
    if(f.empty()) return FPS(precision,0);
    std::vector<T> weights(width);
    if(zero_constant){
      std::copy_n(f.begin(),std::min<int>(width,f.size()),weights.begin());
    }else{
      const int m = f.size();
      auto inverse = denominator.inv(m);
      std::reverse(f.begin(),f.end());
      auto coefficients = fps_product(f,inverse);
      for(int i = 0; i < std::min(m,width); ++i) weights[i] = coefficients[m-1-i];
    }
    for(auto it = kernels.rbegin(); it != kernels.rend(); ++it)
      weights = it->transpose(std::move(weights));
    std::reverse(weights.begin(),weights.end());
    weights.resize(precision);
    return FPS(std::move(weights));
  }

  static FPS project(const FPS& f, const FPS& g, int n, int m){
    if(m == 0 || n == 0 || g.empty()) return FPS(m,0);
    const bool zero_constant = f.empty() || f[0] == T(0);
    const int count = zero_constant ? std::min(n,m) : m;
    const int w0 = soryuusi::bit_ceil(static_cast<unsigned int>(n));
    int p_rows = 1, q_rows = std::min(2,count);
    auto q = initial_denominator(f,w0,q_rows);
    std::vector<T> p(w0);
    std::copy_n(g.begin(),std::min<int>(n,g.size()),p.begin()+(w0-n));
    for(int w = w0; w > 1; w /= 2){
      const Kernel kernel(q,w);
      p_rows = std::min(2*p_rows,count);
      q_rows = std::min(2*q_rows-1,count);
      p = kernel.next_numerator(p,p_rows);
      q = kernel.next_denominator(q_rows);
    }
    FPS result(std::move(p));
    if(!zero_constant) result.pred(count) /= FPS(std::move(q));
    result.resize(m);
    return result;
  }
};

// At each step the x width is halved and the number of y frequencies doubles.
template<class FPS>
class FPSCompositionNTT {
  using T = typename FPS::value_type;
  using NTT = NTTEngine<T>;
  int precision = 0, padded_size = 0;
  std::vector<std::vector<FPS>> denominators;

public:
  void init(const FPS& g, int m){
    precision = m;
    if(m < 2) return;
    padded_size = soryuusi::bit_ceil(static_cast<unsigned int>(m));
    FPS initial = -g;
    initial.resize(padded_size);
    std::vector<FPS> current;
    current.push_back(std::move(initial));
    denominators.reserve(soryuusi::countr_zero(static_cast<unsigned int>(padded_size)));
    for(int width = padded_size, rows = 1; width > 1; width /= 2, rows *= 2){
      auto& layer = denominators.emplace_back(2*rows);
      for(int i = rows; i < 2*rows; ++i) layer[i].resize(2*width);
      if(rows == 1){
        std::copy_n(current[0].begin(),width,layer[1].begin());
      }else{
        std::vector<T> column(rows);
        for(int j = 0; j < width; ++j){
          for(int i = 0; i < rows; ++i) column[i] = current[i][j];
          NTT::ntt_odd_extend(column);
          for(int i = 0; i < rows; ++i) layer[rows+i][j] = column[i];
        }
      }
      for(int i = 0; i < rows; ++i){
        layer[i] = std::move(current[i]);
        layer[i].resize(2*width);
      }
      for(int i = 0; i < 2*rows; ++i){
        layer[i][0] += i < rows ? T(1) : T(-1);
        NTT::ntt(layer[i]);
      }
      if(width == 2) break;
      current.resize(2*rows);
      for(int i = 0; i < 2*rows; ++i){
        auto& row = current[i];
        row.resize(width);
        for(int j = 0; j < width; ++j) row[j] = layer[i][2*j]*layer[i][2*j+1];
        NTT::intt_unnormalized(row);
        NTT::normalize(row.begin(),row.begin()+width/2,width);
        row.resize(width/2);
        row[0] -= T(1);
      }
    }
  }

  FPS composition(FPS f) const {
    if(precision == 0) return {};
    if(f.empty()) return FPS(precision,0);
    if(precision == 1) return FPS{f[0]};
    f.resize(padded_size);
    // The final denominator is 1 because the inner series has zero constant.
    std::rotate(f.begin(),f.end()-1,f.end());
    NTT::ntt(f);
    const T y_scale = NTT::inverse_size(padded_size);
    std::vector<FPS> p(padded_size);
    for(int i = 0; i < padded_size; ++i) p[i] = FPS{f[i]*y_scale};

    int width = 2;
    for(auto it = denominators.rbegin(); it != denominators.rend(); ++it, width *= 2){
      const auto& layer = *it;
      const int rows = layer.size()/2;
      std::vector<FPS> next(2*rows);
      std::vector<T> phase(width);
      phase[0] = NTT::inverse_size(2*width);
      for(int i = 1; i < width; ++i)
        phase[i] = phase[i-1]*NTT::inv_rate2(soryuusi::countr_zero(static_cast<unsigned int>(i)));
      for(int i = 0; i < 2*rows; ++i){
        p[i].resize(width);
        std::reverse(p[i].begin()+1,p[i].end());
        NTT::ntt(p[i]);
        auto& row = next[i];
        row.resize(2*width);
        for(int j = 0; j < width; ++j){
          const T x = p[i][j]*phase[j];
          row[2*j] = x*layer[i][2*j+1];
          row[2*j+1] = -x*layer[i][2*j];
        }
        NTT::intt_unnormalized(row);
        // Only the first width entries of the cyclic reversal are used.
        for(int j = 1; j < width; ++j) row[j] = row[2*width-j];
      }
      p.resize(rows);
      for(int i = 0; i < rows; ++i){
        next[i].resize(width);
        p[i] = std::move(next[i]);
      }
      if(rows == 1){
        for(int j = 0; j < width; ++j) p[0][j] += next[1][j];
      }else{
        std::vector<T> column(rows);
        for(int j = 0; j < width; ++j){
          for(int i = 0; i < rows; ++i) column[i] = next[rows+i][j];
          NTT::ntt_odd_extend_transposed(column);
          for(int i = 0; i < rows; ++i) p[i][j] += column[i];
        }
      }
    }
    auto result = std::move(p[0]);
    std::reverse(result.begin(),result.end());
    result.resize(precision);
    return result;
  }
};

template<class FPS, bool Native = is_ntt_friendly_v<typename FPS::value_type>>
class FPSCompositionPlan {
  using T = typename FPS::value_type;
  int precision;
  std::optional<T> constant, linear;
  std::variant<FPSCompositionCRT<FPS>,std::conditional_t<Native,FPSCompositionNTT<FPS>,std::monostate>> plan;

public:
  FPSCompositionPlan(const FPS& g, int m) : precision(m){
    assert(m >= 0);
    if(m == 0) return;
    const int length = std::min<int>(m,g.size());
    int degree = length-1;
    while(degree > 0 && g[degree] == T(0)) --degree;
    if(degree <= 0){
      constant = length == 0 ? T(0) : g[0];
      return;
    }
    if(degree == 1 && g[0] == T(0)){
      linear = g[1];
      return;
    }
    if constexpr (Native){
      if((g.empty() || g[0] == T(0)) && m <= (1 << (NTTEngine<T>::rank2-1))){
        plan.template emplace<1>().init(g,m);
        return;
      }
    }
    std::get<0>(plan).init(g,m);
  }
  FPS composition(FPS f) const {
    if(precision == 0) return {};
    if(constant){
      const T value = f.eval(*constant);
      f.assign(precision,T(0));
      f[0] = value;
      return f;
    }
    if(linear){
      T scale = 1;
      for(int i = 0; i < std::min<int>(precision,f.size()); ++i){
        f[i] *= scale;
        scale *= *linear;
      }
      f.resize(precision);
      return f;
    }
    if constexpr (Native){
      if(const auto* ntt = std::get_if<1>(&plan)) return ntt->composition(std::move(f));
    }
    return std::get<0>(plan).composition(std::move(f));
  }
};

}  // namespace soryuusi::internal
