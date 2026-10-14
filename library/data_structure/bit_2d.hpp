// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/bit_2d.hpp
#pragma once
#include <vector>

namespace soryuusi {

using namespace std;

template<class T>
struct BIT2D{
  private:
    int h, w; vector<T> s;
    inline int id(int i, int j){return i * (w + 1) + j;}
  public:
    BIT2D(int _h, int _w) : h(_h), w(_w), s((h + 1) * (w + 1), 0) {}
    BIT2D(const vector<vector<T>>& v) : h(v.size()), w(v[0].size()), s((h + 1) * (w + 1), 0) {
      for(int i = 0; i < h; ++i) for(int j = 0; j < w; ++j){
        s[id(i+1,j+1)] += v[i][j];
        int i2 = i + (i & -i), j2 = j + (j & -j);
        if(i2 <= h) s[id(i2,j)] += s[id(i,j)];
        if(j2 <= w) s[id(i,j2)] += s[id(i,j)];
        if(i2 <= h && j2 <= w) s[id(i2,j2)] -= s[id(i,j)];
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
