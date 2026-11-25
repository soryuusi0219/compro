// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/wavelet_bit.hpp
#pragma once
#include <algorithm>
#include <limits>
#include <type_traits>
#include <utility>
#include <vector>

#include "bit.hpp"
#include "static_bit_vector.hpp"
#include "internal/wavelet_partition.hpp"

namespace soryuusi {

using namespace std;

template<class T, class S, int B = 31>
struct Wavelet_BIT{
  private:
    int n;
    int z[B];
    int aggregate[B];
    StaticBitVector data[B];
    BIT<S> fw[B];
  public:
    Wavelet_BIT(vector<T> v, vector<S> w) : n(v.size()) {
      internal::WaveletPartition<T> partition(v);
      vector<S> next_weights;
      next_weights.reserve(n);
      int slot = -1;
      for(int k = B-1; k >= 0; --k) {
        next_weights.clear();
        z[k] = partition.split(v,k,data[k],[&](int i){ next_weights.push_back(w[i]); });
        if(z[k] != 0 && z[k] != n) {
          w.swap(next_weights);
          slot = k;
        }
        if(slot < 0) slot = k;
        if(slot == k) fw[k] = BIT(w);
        aggregate[k] = slot;
      }
    }

    void add(int i, S x){
      for(int k = B-1; k >= 0; --k){
        if(aggregate[k] != k) continue;
        int i0 = data[k].rank0(i);
        if(data[k].get(i)) i += z[k] - i0;
        else i = i0;
        fw[k].add(i,x);
      }
    }

    T get(int i){
      T res = 0;
      for(int k = B-1; k >= 0; k--){
        res <<= 1;
        int i0 = data[k].rank0(i);
        if(data[k].get(i)) res++, i += z[k] - i0;
        else i = i0;
      }
      return res;
    }

    int rank(int l, int r, T x){
      if constexpr(std::is_signed_v<T>) if(x < 0) return 0;
      if constexpr(std::is_integral_v<T> && B < std::numeric_limits<T>::digits) {
        if(x >> B) return 0;
      }
      for(int k = B-1; k >= 0 && l < r; k--){
        int l0 = data[k].rank0(l), r0 = data[k].rank0(r);
        if((x >> k) & 1) l += z[k] - l0, r += z[k] - r0;
        else l = l0, r = r0;
      }
      return r-l;
    }

    S sum_with_val(int l, int r, T x){
      if constexpr(std::is_signed_v<T>) if(x < 0) return S(0);
      if constexpr(std::is_integral_v<T> && B < std::numeric_limits<T>::digits) {
        if(x >> B) return S(0);
      }
      for(int k = B-1; k >= 0 && l < r; k--){
        int l0 = data[k].rank0(l), r0 = data[k].rank0(r);
        if((x >> k) & 1) l += z[k] - l0, r += z[k] - r0;
        else l = l0, r = r0;
      }
      return fw[aggregate[0]].sum(l,r);
    }

    T range_kth(int l, int r, int x){
      T res = 0;
      for(int k = B-1; k >= 0; k--){
        res <<= 1;
        int l0 = data[k].rank0(l), r0 = data[k].rank0(r);
        if(x >= r0 - l0) res++, x -= r0 - l0, l += z[k] - l0, r += z[k] - r0;
        else l = l0, r = r0;
      }
      return res;
    }

    int count_lt(int l, int r, T x){
      if constexpr(std::is_integral_v<T>) {
        if(x <= 0) return 0;
        if constexpr(B < std::numeric_limits<T>::digits) {
          if(x >> B) return r-l;
        }
      }
      int res = 0;
      for(int k = B-1; k >= 0 && l < r; k--){
        int l0 = data[k].rank0(l), r0 = data[k].rank0(r);
        if((x >> k) & 1) res += r0 - l0, l += z[k] - l0, r += z[k] - r0;
        else l = l0, r = r0;
      }
      return res;
    }

    S sum_lt(int l, int r, T x){
      if constexpr(std::is_integral_v<T>) {
        if(x <= 0) return S(0);
        if constexpr(B < std::numeric_limits<T>::digits) {
          if(x >> B) {
            int l0 = data[B-1].rank0(l), r0 = data[B-1].rank0(r);
            const auto& tree = fw[aggregate[B-1]];
            S total = tree.sum(l0,r0);
            total += tree.sum(z[B-1]+l-l0,z[B-1]+r-r0);
            return total;
          }
        }
      }
      S res = 0;
      for(int k = B-1; k >= 0 && l < r; k--){
        int l0 = data[k].rank0(l), r0 = data[k].rank0(r);
        if((x >> k) & 1) res += fw[aggregate[k]].sum(l0,r0), l += z[k] - l0, r += z[k] - r0;
        else l = l0, r = r0;
      }
      return res;
    }

    int count_interval(int l, int r, T xl, T xu){return count_lt(l,r,xu) - count_lt(l,r,xl);}
    S sum_interval(int l, int r, T xl, T xu){return sum_lt(l,r,xu) - sum_lt(l,r,xl);}
    
    // min v[i] s.t. v[i] >= x
    T next(int l, int r, T x, T default_value = -1){
      int cnt = count_lt(l,r,x);
      return cnt == r-l ? default_value : range_kth(l,r,cnt);
    }
    
    // max v[i] s.t. v[i] < x
    T prev(int l, int r, T x, T default_value = -1){
      int cnt = count_lt(l,r,x);
      return cnt == 0 ? default_value : range_kth(l,r,cnt-1);
    }
};

}  // namespace soryuusi
