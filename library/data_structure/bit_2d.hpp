// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/bit_2d.hpp
#pragma once
#include <cstddef>
#include <vector>

namespace soryuusi {

using namespace std;

template<class T>
struct BIT2D{
  private:
    int h, w;
    size_t stride;
    vector<T> s;
    inline size_t id(int i, int j) const {return i * stride + j;}
  public:
    BIT2D(int _h, int _w) : h(_h), w(_w), stride(size_t(w)+1), s((size_t(h)+1)*stride,0) {}
    BIT2D(const vector<vector<T>>& v) : BIT2D(v.size(),v.empty() ? 0 : v[0].size()) {
      for(int i = 1; i <= h; ++i) {
        const size_t row = id(i,0);
        for(int j = 1; j <= w; ++j) {
          s[row+j] += v[i-1][j-1];
          int step = j & -j;
          if(step <= w-j) s[row+j+step] += s[row+j];
        }
      }
      for(int i = 1; i <= h; ++i) {
        int step = i & -i;
        if(step > h-i) continue;
        const size_t row = id(i,0), parent = id(i+step,0);
        for(int j = 1; j <= w; ++j) s[parent+j] += s[row+j];
      }
    }
    void add(int i, int j, T x) {
      for(++i; i <= h; i += i & -i) for(int jj = j+1; jj <= w; jj += jj & -jj) s[id(i,jj)] += x;
    }
    T sum(int i, int j) {
      T res = 0;
      for(; i; i -= i & -i) for(int jj = j; jj; jj -= jj & -jj) res += s[id(i,jj)];
      return res;
    }
    T sum(int hl, int hr, int wl, int wr){return sum(hr,wr) - sum(hr,wl) - sum(hl,wr) + sum(hl,wl);}
};

}  // namespace soryuusi
