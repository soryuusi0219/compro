// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/static_range_inversions.hpp
#pragma once
#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cmath>
#include <functional>
#include <limits>
#include <numeric>
#include <utility>
#include <variant>
#include <vector>

#include "bit.hpp"

namespace soryuusi {

using namespace std;


template<class T, class Compare = less<T>>
struct StaticRangeInversions{
  using ll = long long;
  private:
    using Half = std::uint32_t;
    template<class Cell> using Rows = vector<vector<Cell>>;
    int n, l = 1, k = 0;
    bool nondecreasing = true, decreasing = true;
    vector<int> a, b;
    variant<Rows<Half>,Rows<ll>> table;
    Rows<ll> aligned;
    int boundary(int block) const {return block == k ? n : block*l;}

    template<class Cell> void build(Rows<Cell>& table){
      table.reserve(std::size_t(k)+1);
      for(int i = 0; i <= k; ++i) table.emplace_back(std::size_t(n)+1,Cell{});
      BIT<int> fw(n);
      for(int i = 0, j = 0, j2 = 0; i < k; ++i){
        ll invs = 0;
        while(j < boundary(i+1)){
          invs += j-i*l-fw.sum(a[j]);
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
        int ymx = boundary(i+1);
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
    template<class Cell> ll query_rows(const Rows<Cell>& table, int x, int y) const {
      int xb = x/l, yb = y/l;
      int lb = boundary(xb), le = boundary(xb+1), rb = boundary(yb);
      auto prefix = [&](int from) -> ll {
        if constexpr(sizeof(Cell) == sizeof(Half)){
          if(!aligned.empty()) return aligned[from][yb-from]+Half(table[from][y]-table[from][rb]);
        }
        return table[from][y];
      };
      if(x == lb) return prefix(xb);
      if(xb < yb){
        ll res = prefix(xb+1)+Cell(table[yb][x]-table[yb][le]);
        if(y == rb) return res;
        int i = lb, j = rb, re = boundary(yb+1), count = 0;
        while(i < le || j < re){
          if(j == re || (i < le && a[b[i]] < a[b[j]])){
            if(b[i++] >= x) res += count;
          } else count += (b[j++] < y);
        }
        return res;
      }
      Cell res = table[xb][y]+table[xb+1][x]-table[xb+1][lb];
      for(int i = lb, count = 0; i < le; ++i){
        if(b[i] < x) res += count;
        else if(b[i] >= y) ++count;
      }
      return res;
    }

  public:
    StaticRangeInversions(const vector<T>& v, int q = -1) : n(v.size()){
      assert(q >= -1);
      Compare comp;
      for(int i = 1; i < n && (nondecreasing || decreasing); ++i){
        bool falls = comp(v[i],v[i-1]);
        nondecreasing &= !falls;
        decreasing &= falls;
      }
      if(nondecreasing || decreasing) return;
      if(q == -1) q = n;
      l = q == 0 ? n : static_cast<int>(std::clamp(0.9L*n/std::sqrt(static_cast<long double>(q)),1.0L,static_cast<long double>(n)));
      k = n/l+(n%l != 0);
      a.resize(n);
      b.resize(n);
      iota(b.begin(),b.end(),0);
      stable_sort(b.begin(),b.end(),[&](int i,int j){return comp(v[i],v[j]);});
      for(int i = 0; i < n; ++i) a[b[i]] = i;
      iota(b.begin(),b.end(),0);
      for(int i = 0; i < k; ++i){
        sort(b.begin()+i*l,b.begin()+boundary(i+1),[&](int i,int j){return a[i] < a[j];});
      }
      const ll max_inversions = ll(n)*(n-1)/2;
      constexpr auto limit = std::numeric_limits<Half>::max();
      if(max_inversions > limit && ll(l)*n > limit){
        build(table.emplace<Rows<ll>>());
      } else {
        auto& narrow = get<Rows<Half>>(table);
        build(narrow);
        if(max_inversions > limit){
          // Each increment adds at most one block, so its wrapped difference is exact.
          aligned.resize(std::size_t(k)+1);
          aligned[k].push_back(0);
          for(int i = 0; i < k; ++i){
            auto& row = aligned[i];
            row.resize(std::size_t(k-i)+1);
            for(int j = i+1; j <= k; ++j){
              Half delta = narrow[i][boundary(j)]-narrow[i][boundary(j-1)];
              row[j-i] = row[j-i-1]+delta;
            }
          }
        }
      }
    }
    long long query(int x, int y){
      assert(0 <= x && x <= y && y <= n);
      if(y-x < 2 || nondecreasing) return 0;
      if(decreasing) return ll(y-x)*(y-x-1)/2;
      if(y-x <= 32){
        ll res = 0;
        for(int j = x+1; j < y; ++j) for(int i = x; i < j; ++i) res += a[i] > a[j];
        return res;
      }
      if(auto* narrow = get_if<Rows<Half>>(&table)) return query_rows(*narrow,x,y);
      return query_rows(get<Rows<ll>>(table),x,y);
    }
};

}  // namespace soryuusi
