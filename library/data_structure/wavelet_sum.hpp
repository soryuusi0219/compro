// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/wavelet_sum.hpp
#pragma once
#include <limits>
#include <type_traits>
#include <utility>
#include <vector>

#include "static_bit_vector.hpp"
#include "internal/wavelet_partition.hpp"

namespace soryuusi {

using namespace std;

template<class T, class U, int B = 31>
struct Wavelet_Sum{
  private:
    int n;
    int z[B];
    int aggregate[B];
    StaticBitVector data[B];
    vector<U> val[B];
  public:
    Wavelet_Sum(vector<T> v, const vector<U>& w) : n(v.size()) {
      internal::WaveletPartition<T> partition(v);
      const vector<U>* weights = &w;
      int slot = -1;
      for(int k = B-1; k >= 0; --k) {
        auto& next = val[k];
        int offset = slot < 0 ? 0 : 1;
        z[k] = partition.split(v,k,data[k],[&](int i){
          if(next.empty()) {
            next.reserve(n+1);
            next.push_back(0);
          }
          next.push_back((*weights)[i+offset]);
        });
        if(z[k] == 0 || z[k] == n) {
          if(slot >= 0) {
            aggregate[k] = slot;
            continue;
          }
          next.reserve(n+1);
          next.push_back(0);
          next.insert(next.end(),w.begin(),w.end());
        }
        if(slot >= 0) {
          auto& prefix = val[slot];
          for(int i = 0; i < n; ++i) prefix[i+1] += prefix[i];
        }
        slot = aggregate[k] = k;
        weights = &next;
      }
      auto& prefix = val[slot];
      for(int i = 0; i < n; ++i) prefix[i+1] += prefix[i];
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

    U sum_with_val(int l, int r, T x){
      if constexpr(std::is_signed_v<T>) if(x < 0) return U(0);
      if constexpr(std::is_integral_v<T> && B < std::numeric_limits<T>::digits) {
        if(x >> B) return U(0);
      }
      for(int k = B-1; k >= 0 && l < r; k--){
        int l0 = data[k].rank0(l), r0 = data[k].rank0(r);
        if((x >> k) & 1) l += z[k] - l0, r += z[k] - r0;
        else l = l0, r = r0;
      }
      return val[aggregate[0]][r]-val[aggregate[0]][l];
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

    U sum_lt(int l, int r, T x){
      if constexpr(std::is_integral_v<T>) {
        if(x <= 0) return U(0);
        if constexpr(B < std::numeric_limits<T>::digits) {
          if(x >> B) {
            int l0 = data[B-1].rank0(l), r0 = data[B-1].rank0(r);
            const auto& prefix = val[aggregate[B-1]];
            U total = prefix[r0]-prefix[l0];
            total += prefix[z[B-1]+r-r0]-prefix[z[B-1]+l-l0];
            return total;
          }
        }
      }
      U res = 0;
      for(int k = B-1; k >= 0 && l < r; k--){
        int l0 = data[k].rank0(l), r0 = data[k].rank0(r);
        if((x >> k) & 1) res += val[aggregate[k]][r0] - val[aggregate[k]][l0], l += z[k] - l0, r += z[k] - r0;
        else l = l0, r = r0;
      }
      return res;
    }

    int count_interval(int l, int r, T xl, T xu){return count_lt(l,r,xu) - count_lt(l,r,xl);}
    U sum_interval(int l, int r, T xl, T xu){return sum_lt(l,r,xu) - sum_lt(l,r,xl);}
    
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
