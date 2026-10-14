// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/wavelet_segtree.hpp
#pragma once
#include <iterator>
#include <utility>
#include <vector>

#include "library/data_structure/segtree.hpp"
#include "library/data_structure/static_bit_vector.hpp"

namespace soryuusi {

using namespace std;

template<class T, class U, U (*op)(U,U), U (*e)(), int B = 31>
struct Wavelet_Segtree{
  private:
    int n;
    int z[B];
    StaticBitVector data[B];
    Segtree<U,op,e> seg[B];
  public:
    Wavelet_Segtree(vector<T> v, vector<U> w) : n(v.size()) {
      vector<T> nv;
      vector<U> nw;
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
        seg[k] = Segtree<U,op,e>(w);
      }
    }

    pair<T,U> get(int i){
      T res = 0;
      for(int k = B-1; k >= 0; --k){
        res <<= 1;
        int i0 = data[k].rank0(i);
        if(data[k].get(i)) res++, i += z[k] - i0;
        else i = i0;
      }
      return {res, seg[0].get(i)};
    }
    
    void set(int i, U x){
      for(int k = B-1; k >= 0; --k){
        int i0 = data[k].rank0(i);
        if(data[k].get(i)) i += z[k] - i0;
        else i = i0;
        seg[k].set(i,x);
      }
    }

    int rank(int l, int r, T x){
      for(int k = B-1; k >= 0; --k){
        int l0 = data[k].rank0(l), r0 = data[k].rank0(r);
        if((x >> k) & 1) l += z[k] - l0, r += z[k] - r0;
        else l = l0, r = r0;
      }
      return r-l;
    }

    U prod_with_val(int l, int r, T x){
      for(int k = B-1; k >= 0; --k){
        int l0 = data[k].rank0(l), r0 = data[k].rank0(r);
        if((x >> k) & 1) l += z[k] - l0, r += z[k] - r0;
        else l = l0, r = r0;
      }
      return seg[0].prod(l,r);
    }

    T range_kth(int l, int r, int x){
      T res = 0;
      for(int k = B-1; k >= 0; --k){
        res <<= 1;
        int l0 = data[k].rank0(l), r0 = data[k].rank0(r);
        if(x >= r0 - l0) res++, x -= r0 - l0, l += z[k] - l0, r += z[k] - r0;
        else l = l0, r = r0;
      }
      return res;
    }

    int count_lt(int l, int r, T x){
      int res = 0;
      for(int k = B-1; k >= 0; --k){
        int l0 = data[k].rank0(l), r0 = data[k].rank0(r);
        if((x >> k) & 1) res += r0 - l0, l += z[k] - l0, r += z[k] - r0;
        else l = l0, r = r0;
      }
      return res;
    }

    U prod_lt(int l, int r, T x){
      U res = e();
      for(int k = B-1; k >= 0; --k){
        int l0 = data[k].rank0(l), r0 = data[k].rank0(r);
        if((x >> k) & 1) res = op(res,seg[k].prod(l0,r0)), l += z[k] - l0, r += z[k] - r0;
        else l = l0, r = r0;
      }
      return res;
    }

    U prod_geq(int l, int r, T x){
      U res = e();
      for(int k = B-1; k >= 0; --k){
        int l0 = data[k].rank0(l), r0 = data[k].rank0(r);
        if((x >> k) & 1) l += z[k] - l0, r += z[k] - r0;
        else res = op(res,seg[k].prod(l + z[k] - l0, r + z[k] - r0)), l = l0, r = r0;
      }
      return op(res,seg[0].prod(l,r));
    }

    int count_interval(int l, int r, T xl, T xu){return count_lt(l,r,xu) - count_lt(l,r,xl);}
    U prod_interval(int l, int r, T xl, T xu){
      if(xl == xu) return e();
      U res = e();
      int k = B-1;
      for(; k >= 0; --k){
        int l0 = data[k].rank0(l), r0 = data[k].rank0(r);
        if(((xl^xu) >> k) & 1){
          l += z[k] - l0, r += z[k] - r0;
          for(--k; k >= 0; --k){
            int l1 = data[k].rank0(l0), r1 = data[k].rank0(r0);
            if((xl >> k) & 1) l0 += z[k] - l1, r0 += z[k] - r1;
            else res = op(res,seg[k].prod(l0 + z[k] - l1, r0 + z[k] - r1)), l0 = l1, r0 = r1;
            int l2 = data[k].rank0(l), r2 = data[k].rank0(r);
            if((xu >> k) & 1) res = op(res,seg[k].prod(l2,r2)), l += z[k] - l2, r += z[k] - r2;
            else l = l2, r = r2;
          }
          return op(res,seg[0].prod(l0,r0));
        }else{
          if((xl >> k) & 1) l += z[k] - l0, r += z[k] - r0;
          else l = l0, r = r0;
        }
      }
      return e();
    }
    
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
