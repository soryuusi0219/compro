// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/static_range_inversions.hpp
#pragma once
#include <algorithm>
#include <cmath>
#include <functional>
#include <numeric>
#include <utility>
#include <vector>

namespace soryuusi {

using namespace std;


template<class T, class Compare = less<T>>
struct StaticRangeInversions{
  using ll = long long;
  private:
  struct BIT{
      private:
        int n; vector<ll> s;
      public:
        BIT() {}
        BIT(int _n) : n(_n), s(_n + 1, 0) {}
        void clear(){fill(s.begin(),s.end(),0);}
        void clear(int n_){n = n_; s.resize(n+1);}
        void add(int p, int x) {
          for(++p; p <= n; p += p & -p) s[p] += x;
        }
        ll sum(int p) const {
          ll res = 0;
          for(; p; p -= p & -p) res += s[p];
          return res;
        }
        ll sum(int l, int r) const { return sum(r) - sum(l); }
    };
    int n, l, k;
    vector<int> a, b;
    vector<vector<ll>> table;
    
  public:
    StaticRangeInversions(const vector<T>& v, int q = -1) : n(v.size()), a(n), b(n){
      if(q == -1) q = n;
      if(!n) return;
      l = min<int>(max<int>(static_cast<long double>(n) / sqrtl(q) * 0.9L,1),n);
      k = (n+l-1)/l;
      iota(b.begin(), b.end(), 0);
      sort(b.begin(), b.end(), [&](int l, int r){return v[l] == v[r] ? l < r : Compare()(v[l],v[r]);});
      for(int i = 0; i < n; ++i) a[b[i]] = i;
      iota(b.begin(),b.end(),0);
      for(int i = 0; i < k; ++i) sort(b.begin() + i*l, b.begin() + min((i+1)*l,n), [&](int l, int r){return a[l] < a[r];});
      table = vector<vector<ll>>(k+1,vector<ll>(n+1,0));
      BIT fw(n);
      for(int i = 0, j = 0, j2 = 0; i < k; ++i){
        ll invs = 0;
        while(j < min((i+1)*l,n)){
          invs += fw.sum(a[j],n);
          fw.add(a[j],1);
          table[i][++j] = invs;
        }
        for(; j2 < j; ++j2)  fw.add(a[j2],-1);
      }
      vector<int> lblock(b.rbegin(),b.rbegin() + (n-(k-1)*l)), nlblock;
      lblock.reserve(n), nlblock.reserve(n);
      for(int i = k-2, y = (k-1)*l; i >= 0; --i){
        nlblock.clear();
        int x = 0;
        int xmx = lblock.size();
        while(x < xmx || y > i*l){
          if(y == i*l || (x < xmx && a[lblock[x]] > a[b[y-1]])){
            table[i][lblock[x]+1] = (i+1)*l - y;
            nlblock.push_back(lblock[x++]);
          }else nlblock.push_back(b[--y]);
        }
        for(int j = (i+1)*l; j < n; ++j) table[i][j+1] += table[i][j] + table[i+1][j+1] - table[i+1][j];
        lblock.swap(nlblock);
      }
      for(int i = k, j = n, j2 = n; i; --i){
        ll invs = 0;
        while(j > (i-1)*l){
          invs += fw.sum(a[--j]);
          fw.add(a[j],1);
          table[i][j] = invs;
        }
        while(j2 > j)  fw.add(a[--j2],-1);
      }
      lblock.clear();
      lblock.insert(lblock.begin(), b.begin(), b.begin() + l);
      for(int i = 1, y = l; i < k; ++i){
        nlblock.clear();
        int x = 0;
        int xmx = lblock.size();
        int ymx = min((i+1)*l,n);
        while(x < xmx || y < ymx){
          if(y == ymx || (x < xmx && a[lblock[x]] < a[b[y]])){
            table[i+1][lblock[x]] = y - i*l;
            nlblock.push_back(lblock[x++]);
          }else nlblock.push_back(b[y++]);
        }
        for(int j = i*l; j; --j) table[i+1][j-1] += table[i+1][j] + table[i][j-1] - table[i][j];
        lblock.swap(nlblock);
      }
    }
    long long query(int x, int y){
      if(y-x < 2) return 0;
      int xb = x/l, yb = y/l;
      if(xb < yb){
        ll res = table[xb+1][y] + table[yb][x] - table[yb][(xb+1)*l];
        int i = xb*l, j = yb*l, imx = (xb+1)*l, jmx = min((yb+1)*l,n), cnt = 0;
        while(i < imx || j < jmx){
          if(j == jmx || (i < imx && a[b[i]] < a[b[j]])) {
            if(b[i++] >= x) res += cnt;
          }else cnt += (b[j++] < y);
        }
        return res;
      }else{
        ll res = table[xb][y] + table[xb+1][x] - table[xb+1][xb*l];
        for(int i = xb*l, cnt = 0; i < min((xb+1)*l,n); ++i){
          if(b[i] < x) res += cnt;
          else if(b[i] >= y) ++cnt;
        }
        return res;
      }
    }
};

}  // namespace soryuusi
