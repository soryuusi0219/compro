// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/dynamic_wavelet.hpp
#pragma once
#include <algorithm>
#include <cassert>
#include <utility>
#include <vector>
#include "dynamic_bit_vector.hpp"

namespace soryuusi {

using namespace std;

template<class T, int B = 31>
struct DynamicWavelet{
  private:
    int n;
    int z[B];
    DynamicBitVector data[B];
  public:
    DynamicWavelet(const DynamicWavelet&) = delete;
    DynamicWavelet& operator=(const DynamicWavelet&) = delete;
    DynamicWavelet(DynamicWavelet&& other) noexcept : n(std::exchange(other.n,0)){
      for(int k = 0; k < B; ++k){z[k] = std::exchange(other.z[k],0); data[k] = std::move(other.data[k]);}
    }
    DynamicWavelet& operator=(DynamicWavelet&& other) noexcept {
      if(this != &other){n = std::exchange(other.n,0); for(int k = 0; k < B; ++k){z[k] = std::exchange(other.z[k],0); data[k] = std::move(other.data[k]);}}
      return *this;
    }
    DynamicWavelet(vector<T> v) : n(v.size()){
      vector<T> output;
      for(int k = B-1; k >= 0; --k){
        data[k].build_blocks(n,[&](int start, int length){
          DynamicBitVector::Word bits = 0;
          for(int j = 0; j < length; ++j) bits |= DynamicBitVector::Word(!((v[start+j]>>k)&1))<<j;
          return bits;
        });
        z[k] = data[k].rank(n);
        if(k && 0 < z[k] && z[k] < n){
          if(output.empty()) output = v;
          int left = 0, right = z[k];
          for(const T& x : v){
            bool zero = !((x>>k)&1);
            output[zero ? left : right] = x;
            left += zero; right += !zero;
          }
          v.swap(output);
        }
      }
    }

    int size(){return n;}

    [[nodiscard]] DynamicWavelet clone(){
      DynamicWavelet result(vector<T>{});
      result.n = n;
      for(int k = 0; k < B; ++k){result.z[k] = z[k]; result.data[k] = data[k].clone();}
      return result;
    }
    T get(int i){
      assert(0 <= i && i < n);
      T res = 0;
      for(int k = B-1; k >= 0; --k){
        res <<= 1;
        auto [i0,zero] = data[k].get_and_rank(i);
        if(!zero){++res; i += z[k]-i0;}
        else i = i0;
      }
      return res;
    }

    void insert(int i, T x){
      assert(0 <= i && i <= n);
      for(int k = B-1; k >= 0; --k){
        bool zero = !((x >> k) & 1);
        int i0 = data[k].template insert_impl<true>(i,zero);
        if(zero){i = i0; ++z[k];}
        else i += z[k]-i0;
      }
      ++n;
    }

    void erase(int i){
      assert(0 <= i && i < n);
      for(int k = B-1; k >= 0; --k){
        auto [i0,zero] = data[k].template erase_impl<true>(i);
        if(zero){i = i0; --z[k];}
        else i += z[k]-i0;
      }
      --n;
    }

    void set(int i, T x){
      assert(0 <= i && i < n);
      for(int k = B-1; k >= 0; --k){
        bool zero = !((x >> k) & 1);
        if(z[k] == (zero ? n : 0)) continue;
        auto [i0,old_zero] = data[k].template set_impl<true>(i,zero);
        int old_i = old_zero ? i0 : i+(z[k]-i0);
        if(old_zero == zero){i = old_i; continue;}
        // The new index is in the lower level after removing the old element.
        z[k] += int(zero)-int(old_zero);
        int new_i = zero ? i0 : i+(z[k]-i0);
        for(--k; k >= 0; --k){
          zero = !((x >> k) & 1);
          if(z[k] == (zero ? n : 0)) continue;
          auto [old_rank,was_zero] = data[k].template erase_impl<true>(old_i);
          if(was_zero){old_i = old_rank; --z[k];}
          else old_i += z[k]-old_rank;
          int new_rank = data[k].template insert_impl<true>(new_i,zero);
          if(zero){new_i = new_rank; ++z[k];}
          else new_i += z[k]-new_rank;
        }
        return;
      }
    }

    int rank(int l, int r, T x){
      assert(0 <= l && l <= r && r <= n);
      for(int k = B-1; k >= 0 && l != r; --k){
        auto [l0,r0] = data[k].rank_pair(l,r);
        if((x >> k) & 1) l += z[k] - l0, r += z[k] - r0;
        else l = l0, r = r0;
      }
      return r-l;
    }

    T range_kth(int l, int r, int x){
      assert(0 <= l && l <= r && r <= n && 0 <= x && x < r-l);
      T res = 0;
      for(int k = B-1; k >= 0; k--){
        res <<= 1;
        auto [l0,r0] = data[k].rank_pair(l,r);
        if(x >= r0 - l0) res++, x -= r0 - l0, l += z[k] - l0, r += z[k] - r0;
        else l = l0, r = r0;
      }
      return res;
    }

    int count_lt(int l, int r, T x){
      assert(0 <= l && l <= r && r <= n);
      int res = 0;
      for(int k = B-1; k >= 0 && l != r; --k){
        auto [l0,r0] = data[k].rank_pair(l,r);
        if((x >> k) & 1) res += r0 - l0, l += z[k] - l0, r += z[k] - r0;
        else l = l0, r = r0;
      }
      return res;
    }

    int count_interval(int l, int r, T xl, T xu){return count_lt(l,r,xu) - count_lt(l,r,xl);}

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
