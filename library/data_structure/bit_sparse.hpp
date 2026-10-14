// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/bit_sparse.hpp
#pragma once
#include <algorithm>
#include <vector>

namespace soryuusi {

using namespace std;

template<class T, class X = int>
struct BIT_Sparse{
  private:
    int n;
    vector<T> s;
    vector<X> vx;
  public:
    BIT_Sparse(){}
    BIT_Sparse(int sz){vx.reserve(sz);}
    void add_point(X x){vx.push_back(x);}
    void build(){
      sort(vx.begin(),vx.end());
      vx.erase(unique(vx.begin(),vx.end()),vx.end());
      n = vx.size();
      s.assign(n+1,0);
    }
    void add(X i, T x) {
      int p = lower_bound(vx.begin(),vx.end(),i) - vx.begin();
      for(++p; p <= n; p += p & -p) s[p] += x;
    }
    T sum(X i) const {
      int p = lower_bound(vx.begin(),vx.end(),i) - vx.begin();
      T res = 0;
      for(; p; p -= p & -p) res += s[p];
      return res;
    }
    T sum(X l, X r) const { return sum(r) - sum(l); }
};

}  // namespace soryuusi
