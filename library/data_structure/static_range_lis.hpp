// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/static_range_lis.hpp
#pragma once
#include <algorithm>
#include <cassert>
#include <functional>
#include <numeric>
#include <vector>

#include "library/data_structure/wavelet.hpp"

namespace soryuusi {

using namespace std;

template<class T, class Compare = less<T>>
struct StaticRangeLIS{
  private:
    int n;
    Wavelet<int> wv;

    static vector<int> seaweed_doubling(const vector<T>& v){
      int n = v.size();
      vector<int> a(n);
      iota(a.begin(), a.end(), 0);
      Compare comp;
      sort(a.begin(), a.end(), [&](int l, int r){
        if(comp(v[l],v[r])) return true;
        if(comp(v[r],v[l])) return false;
        return l < r;
      });

      vector<int> reca(n), recb(n), res(n,-1), buff(n), ida(n), idb(n);
      vector<bool> s(n);
      auto rec = [&](auto&& self, int l, int r) -> void {
        if(r - l < 2) return;
        int m = (l + r) >> 1;
        self(self,l,m), self(self,m,r);
        for(int i = l, j = m, k = l; i < m || j < r; ++k){
          if(j == r || (i < m && reca[ida[i]] > reca[ida[j]])) buff[k] = ida[i++];
          else buff[k] = ida[j++];
        }
        copy(buff.begin()+l, buff.begin()+r, ida.begin()+l);
        fill(s.begin()+l,s.begin()+m,true);
        fill(s.begin()+m,s.begin()+r,false);
        int x = l;
        for(int i = l, j = m, k = l; i < m || j < r; ++k){
          if(j == r || (i < m && recb[idb[i]] < recb[idb[j]])) {
            if(s[idb[i]]) {
              res[idb[i]] = recb[idb[i]];
              s[idb[i]] = false;
              buff[k] = idb[i++];
            }
            else{
              while(!s[ida[x]]) ++x;
              res[ida[x]] = recb[idb[i]];
              s[ida[x]] = false;
              buff[k] = ida[x];
              ++i;
            }
          }
          else{
            if(reca[idb[j]] >= reca[ida[x]]) {
              res[idb[j]] = recb[idb[j]];
              buff[k] = idb[j++];
            }
            else{
              s[idb[j]] = true;
              while(!s[ida[x]]) ++x;
              res[ida[x]] = recb[idb[j]];
              s[ida[x]] = false;
              buff[k] = ida[x];
              ++j;
            }
          }
        }
        copy(buff.begin()+l, buff.begin()+r, idb.begin()+l);
        copy(res.begin()+l, res.begin()+r, recb.begin()+l);
      };

      auto seaweed_prod = [&](int l, int r) -> void {
        iota(ida.begin() + l, ida.begin() + r, l);
        iota(idb.begin() + l, idb.begin() + r, l);
        rec(rec,l,r);
        fill(res.begin()+l, res.begin()+r, -1);
        for(int i = l; i < r; ++i) if(recb[i] < r) res[recb[i]] = reca[i];
      };

      auto solve = [&](auto&& self, int l, int r) -> void {
        if(r - l < 2) return;
        int m = (l + r) >> 1;
        self(self,l,m), self(self,m,r);
        for(int i = l, j = m, k = l; i < m || j < r; ++k){
          if(j == r || (i < m && a[i] < a[j])) ida[i] = k, buff[k] = a[i++];
          else ida[j] = k, buff[k] = a[j++];
        }
        copy(buff.begin() + l, buff.begin() + r, a.begin() + l);
        fill(recb.begin() + l, recb.begin() + r, r);
        for(int i = l; i < m; ++i){
          reca[ida[i]] = ~res[i] ? ida[res[i]] : -1;
          recb[ida[i]] = ida[i];
        }
        for(int i = m; i < r; ++i){
          reca[ida[i]] = ida[i];
          if(~res[i]) recb[ida[res[i]]] = ida[i];
        }
        seaweed_prod(l,r);
      };
      solve(solve,0,n);
      for(int i = 0; i < n; ++i) ++res[i];
      return res;
    }
  public:
    StaticRangeLIS(const vector<T>& v) : n(v.size()), wv(seaweed_doubling(v)) {}
    // Equal elements can both be chosen (longest nondecreasing subsequence).
    int query(int l, int r) {
      assert(0 <= l && l <= r && r <= n);
      if(l == r) return 0;
      return wv.count_lt(0,r,l+1) - l;
    }
};

}  // namespace soryuusi
