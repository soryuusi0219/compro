// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/segtree_sparse.hpp
#pragma once
#include <algorithm>
#include <type_traits>
#include <vector>

namespace soryuusi {

using namespace std;

template<class S, auto op, auto e, class X = int>
struct Segtree_Sparse{
  static_assert(is_invocable_r_v<S, decltype(op), S, S>, "op must be callable with (S, S) and return a value convertible to S");
  static_assert(is_invocable_r_v<S, decltype(e)>, "e must be callable with no arguments and return a value convertible to S");

  private:
    int n, b;
    vector<X> vx;
    vector<S> data;
    void _update(int i){data[i] = op(data[i << 1],data[i << 1 | 1]);}
  public:
    Segtree_Sparse(){}
    Segtree_Sparse(int sz){vx.reserve(sz);}
    void add_point(X x){vx.push_back(x);}
    void build(){
      sort(vx.begin(),vx.end());
      vx.erase(unique(vx.begin(),vx.end()),vx.end());
      n = vx.size();
      b = 1; while(b < n) b <<= 1;
      data.assign(b*2,e());
    }
    void set(X x, S a){
      int i = lower_bound(vx.begin(),vx.end(),x) - vx.begin();
      for(data[i += b] = a; i >>= 1;) _update(i);
    }
    S prod(X x, X y){
      int l = lower_bound(vx.begin(),vx.end(),x) - vx.begin();
      int r = lower_bound(vx.begin(),vx.end(),y) - vx.begin();
      S resl = e(), resr = e();
      for(l += b, r += b; l < r; l >>= 1, r >>= 1){
        if(l&1) resl = op(resl, data[l++]);
        if(r&1) resr = op(data[--r], resr);
      }
      return op(resl,resr);
    }
    S all_prod(){return data[1];}
};

}  // namespace soryuusi
