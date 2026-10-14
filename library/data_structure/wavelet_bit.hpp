// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/wavelet_bit.hpp
#pragma once
#include <algorithm>
#include <iterator>
#include <utility>
#include <vector>

#include "library/data_structure/bit.hpp"
#include "library/data_structure/static_bit_vector.hpp"

namespace soryuusi {

using namespace std;

template<class T, class S, int B = 31>
struct Wavelet_BIT{
  private:
    int n;
    int z[B];
    StaticBitVector data[B];
    BIT<S> fw[B+1];
  public:
    Wavelet_BIT(vector<T> v, vector<S> w) : n(v.size()) {
      vector<T> nv;
      vector<S> nw;
      nv.reserve(n);
      nw.reserve(n);
      for(int k = B-1; k >= 0; --k) {
        data[k].build(n);
        nv.clear();
        nw.clear();
        for(int i = 0; i < n; i++) if(!((v[i] >> k) & 1)) nv.push_back(v[i]), nw.push_back(w[i]);
        z[k] = nv.size();
        for(int i = 0; i < n; i++) if((v[i] >> k) & 1) data[k].set(i), nv.push_back(v[i]), nw.push_back(w[i]);
        data[k].init();
        v.swap(nv);
        w.swap(nw);
        fw[k] = BIT(w);
      }
    }

    void add(int i, S x){
      for(int k = B-1; k >= 0; --k){
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
      for(int k = B-1; k >= 0; k--){
        int l0 = data[k].rank0(l), r0 = data[k].rank0(r);
        if((x >> k) & 1) l += z[k] - l0, r += z[k] - r0;
        else l = l0, r = r0;
      }
      return r-l;
    }

    S sum_with_val(int l, int r, T x){
      for(int k = B-1; k >= 0; k--){
        int l0 = data[k].rank0(l), r0 = data[k].rank0(r);
        if((x >> k) & 1) l += z[k] - l0, r += z[k] - r0;
        else l = l0, r = r0;
      }
      return fw[0].sum(l,r);
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

    S sum_lt(int l, int r, T x){
      S res = 0;
      for(int k = B-1; k >= 0; k--){
        int l0 = data[k].rank0(l), r0 = data[k].rank0(r);
        if((x >> k) & 1) res += fw[k].sum(l0,r0), l += z[k] - l0, r += z[k] - r0;
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
