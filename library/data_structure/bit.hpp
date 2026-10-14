// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/bit.hpp
#pragma once
#include <algorithm>
#include <vector>

namespace soryuusi {

using namespace std;

template<class T>
struct BIT{
  private:
    int n; vector<T> s;
  public:
    BIT() {}
    BIT(int _n) : n(_n), s(_n + 1, 0) {}
    BIT(const vector<T>& v) : n(v.size()), s(n + 1, 0) {
      for(int i = 0; i < n; ++i) {
        s[i + 1] += v[i];
        if(i + (i & -i) <= n) s[i + (i & -i)] += s[i];
      }
    }
    void add(int p, T x) {
      for(++p; p <= n; p += p & -p) s[p] += x;
    }
    T sum(int p) const {
      T res = 0;
      for(; p; p -= p & -p) res += s[p];
      return res;
    }
    T sum(int l, int r) const { return sum(r) - sum(l); }
    int lower_bound(T k) const {
      if(k < 0) return -1;
      int x = 0;
      for(int i = 1 << __lg(n); i; i /= 2) {
          if(x + i <= n && s[x + i] <= k) {
            x += i;
            k -= s[x];
          }
      }
      return x;
    }
};

}  // namespace soryuusi
