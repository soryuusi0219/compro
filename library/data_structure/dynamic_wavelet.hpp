// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/dynamic_wavelet.hpp
#pragma once
#include <cassert>
#include <utility>
#include <vector>
#include "library/data_structure/dynamic_bit_vector.hpp"

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
      vector<T> vl, vr;
      vl.reserve(n);
      vr.reserve(n);
      vector<bool> b(n);
      for(int k = B-1; k >= 0; k--) {
        vl.clear();
        vr.clear();
        for(int i = 0; i < n; i++){
          if(b[i] = !((v[i] >> k) & 1)) vl.push_back(v[i]);
          else vr.push_back(v[i]);
        }
        data[k].build(b);
        z[k] = vl.size();
        v.swap(vl);
        v.insert(v.end(), vr.begin(), vr.end());
      }
    }

    int size(){return n;}

    [[nodiscard]] DynamicWavelet clone(){
      vector<T> values(n);
      for(int i = 0; i < n; ++i) values[i] = get(i);
      return DynamicWavelet(std::move(values));
    }
    T get(int i){
      T res = 0;
      for(int k = B-1; k >= 0; k--){
        res <<= 1;
        int i0 = data[k].rank(i);
        if(!data[k].get(i)) res++, i += z[k] - i0;
        else i = i0;
      }
      return res;
    }

    void insert(int i, T x){
      assert(0 <= i && i <= n);
      ++n;
      for(int k = B-1; k >= 0; --k){
        data[k].insert(i,!((x >> k) & 1));
        int i0 = data[k].rank(i);
        if((x >> k) & 1) i += z[k] - i0;
        else i = i0, ++z[k];
      }
    }
  
    void erase(int i){
      assert(0 <= i && i < n);
      --n;
      for(int k = B-1; k >= 0; --k){
        int i0 = data[k].rank(i);
        bool f = !data[k].get(i);
        data[k].erase(i);
        if(f) i += z[k] - i0;
        else i = i0, --z[k];
      }
    }

    void set(int i, T x){erase(i); insert(i,x);}

    int rank(int l, int r, T x){
      if(!n) return 0;
      for(int k = B-1; k >= 0; k--){
        int l0 = data[k].rank(l), r0 = data[k].rank(r);
        if((x >> k) & 1) l += z[k] - l0, r += z[k] - r0;
        else l = l0, r = r0;
      }
      return r-l;
    }

    T range_kth(int l, int r, int x){
      T res = 0;
      for(int k = B-1; k >= 0; k--){
        res <<= 1;
        int l0 = data[k].rank(l), r0 = data[k].rank(r);
        if(x >= r0 - l0) res++, x -= r0 - l0, l += z[k] - l0, r += z[k] - r0;
        else l = l0, r = r0;
      }
      return res;
    }

    int count_lt(int l, int r, T x){
      if(!n) return 0;
      int res = 0;
      for(int k = B-1; k >= 0; k--){
        int l0 = data[k].rank(l), r0 = data[k].rank(r);
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
