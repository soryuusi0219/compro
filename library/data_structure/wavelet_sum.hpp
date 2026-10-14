// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/wavelet_sum.hpp
#pragma once
#include <iterator>
#include <utility>
#include <vector>

#include "library/data_structure/static_bit_vector.hpp"

namespace soryuusi {

using namespace std;

template<class T, class U, int B = 31>
struct Wavelet_Sum{
  private:
    int n;
    int z[B];
    StaticBitVector data[B];
    vector<U> val[B+1];
  public:
    Wavelet_Sum(vector<T> v, const vector<U>& w) : n(v.size()) {
      vector<T> nv;
      nv.reserve(n);
      val[B].reserve(n+1);
      val[B] = {0};
      val[B].insert(val[B].end(), w.begin(), w.end());
      for(int k = B-1; k >= 0; k--) {
        data[k].build(n);
        val[k].reserve(n+1);
        val[k] = {0};
        nv.clear();
        for(int i = 0; i < n; i++) if(!((v[i] >> k) & 1)) nv.push_back(v[i]), val[k].push_back(val[k+1][i+1]);
        z[k] = nv.size();
        for(int i = 0; i < n; i++) if((v[i] >> k) & 1) data[k].set(i), nv.push_back(v[i]), val[k].push_back(val[k+1][i+1]);
        data[k].init();
        v.swap(nv);
      }
      for(int k = 0; k < B; k++) for(int i = 0; i < n; i++) val[k][i+1] += val[k][i];
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
      for(int k = B-1; k >= 0; k--){
        int l0 = data[k].rank0(l), r0 = data[k].rank0(r);
        if((x >> k) & 1) l += z[k] - l0, r += z[k] - r0;
        else l = l0, r = r0;
      }
      return r-l;
    }

    U sum_with_val(int l, int r, T x){
      for(int k = B-1; k >= 0; k--){
        int l0 = data[k].rank0(l), r0 = data[k].rank0(r);
        if((x >> k) & 1) l += z[k] - l0, r += z[k] - r0;
        else l = l0, r = r0;
      }
      return val[0][r]-val[0][l];
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
      int res = 0;
      for(int k = B-1; k >= 0; k--){
        int l0 = data[k].rank0(l), r0 = data[k].rank0(r);
        if((x >> k) & 1) res += r0 - l0, l += z[k] - l0, r += z[k] - r0;
        else l = l0, r = r0;
      }
      return res;
    }

    U sum_lt(int l, int r, T x){
      U res = 0;
      for(int k = B-1; k >= 0; k--){
        int l0 = data[k].rank0(l), r0 = data[k].rank0(r);
        if((x >> k) & 1) res += val[k][r0] - val[k][l0], l += z[k] - l0, r += z[k] - r0;
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
