// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/segtree.hpp
#pragma once
#include <bit>
#include <type_traits>
#include <vector>

namespace soryuusi {

using namespace std;

template<class S, auto op, auto e>
struct Segtree{
  static_assert(is_invocable_r_v<S, decltype(op), S, S>, "op must be callable with (S, S) and return a value convertible to S");
  static_assert(is_invocable_r_v<S, decltype(e)>, "e must be callable with no arguments and return a value convertible to S");
  private:
    int n, b;
    vector<S> data;
    void _update(int i){data[i] = op(data[i << 1],data[i << 1 | 1]);}
  public:
    Segtree(){}
    Segtree(int n_) : n(n_), b(1<<bit_width((unsigned)n)), data(b*2,e()){}
    Segtree(vector<S> v) : n(v.size()), b(1<<bit_width((unsigned)n)), data(b*2,e()){
      for(int i = 0; i < n; ++i) data[b+i] = v[i];
      for(int i = b-1; i; --i) _update(i);
    }
    void set(int i, S a){
      for(data[i += b] = a; i >>= 1;) _update(i);
    }
    void reset(int i){ set(i,e());}
    S get(int i){return data[i + b];}
    S prod(int l, int r){
      S resl = e(), resr = e();
      for(l += b, r += b; l < r; l >>= 1, r >>= 1){
        if(l&1) resl = op(resl, data[l++]);
        if(r&1) resr = op(data[--r], resr);
      }
      return op(resl,resr);
    }
    S all_prod(){return data[1];}

    
    template<class F> int max_right(int l, F f) const {
      if(l == n) return n;
      S sm = e();
      l += b;
      do{
        while(!(l & 1)) l >>= 1;
        if(!f(op(sm, data[l]))) {
          while(l < b) if(f(op(sm, data[l <<= 1]))) sm = op(sm, data[l++]);
          return l - b;
        }
        sm = op(sm, data[l++]);
      }while((l & -l) != l);
      return n;
    }
    template<class F> int min_left(int r, F f) const {
      if(r == 0) return 0;
      S sm = e();
      r += b;
      do{
        r--;
        while(r > 1 && (r & 1)) r >>= 1;
        if(!f(op(data[r], sm))) {
          while(r < b) if(f(op(data[r = r << 1 | 1], sm))) sm = op(data[r--], sm);
          return r + 1 - b;
        }
        sm = op(data[r], sm);
      }while((r & -r) != r);
      return 0;
    }
};

}  // namespace soryuusi
