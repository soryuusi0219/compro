// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/segtree_2d.hpp
#pragma once
#include <cassert>
#include <bit>
#include <vector>

namespace soryuusi {

using namespace std;

template<class S, S (*op)(S,S), S (*e)()>
struct Segtree2D{
  private:
    int h, w, hb, wb;
    vector<S> data;
    inline int id(int i, int j) const {return i * wb * 2 + j;}
    void _update_h(int i, int j) { data[id(i,j)] = op(data[id(i << 1, j)], data[id(i << 1 | 1, j)]); }
    void _update_w(int i, int j) { data[id(i,j)] = op(data[id(i, j << 1)], data[id(i, j << 1 | 1)]); }
  public:
    Segtree2D() : Segtree2D(0,0) {}
    Segtree2D(int h_, int w_) : h(h_), w(w_), hb(bit_ceil((unsigned)h)), wb(bit_ceil((unsigned)w)), data(hb * wb * 4, e()) {}
    Segtree2D(const vector<vector<S>>& v) : h(v.size()), w(v[0].size()), hb(bit_ceil((unsigned)h)), wb(bit_ceil((unsigned)w)), data(hb * wb * 4, e()) {
      for(int i = 0; i < h; ++i) for(int j = 0; j < w; ++j) data[id(i + hb, j + wb)] = v[i][j];
      for(int i = hb - 1; i; --i) for(int j = 0; j < w; ++j) _update_h(i, j + wb);
      for(int i = 0; i < hb + h; ++i) for(int j = wb - 1; j; --j) _update_w(i, j);
    }
    void set(int i, int j, S x){
      assert(0 <= i && i < h && 0 <= j && j < w);
      data[id(i += hb, j += wb)] = x;
      for(int i0 = i; i0 >>= 1;) _update_h(i0, j);
      for(; i; i >>= 1) for(int j0 = j; j0 >>= 1;) _update_w(i, j0);
    }

    S get(int i, int j) const {
      assert(0 <= i && i < h && 0 <= j && j < w);
      return data[id(i + hb, j + wb)];
    }
    S prod(int hl, int hr, int wl, int wr) const {
      assert(0 <= hl && hl <= hr && hr <= h);
      assert(0 <= wl && wl <= wr && wr <= w);
      S res = e();
      for(hl += hb, hr += hb; hl < hr; hl >>= 1, hr >>= 1) {
        if(hl & 1){
          for(int l = wl + wb, r = wr + wb; l < r; l >>= 1, r >>= 1) {
            if(l & 1) res = op(res, data[id(hl, l++)]);
            if(r & 1) res = op(res, data[id(hl, --r)]);
          }
          ++hl;
        }
        if(hr & 1){
          --hr;
          for(int l = wl + wb, r = wr + wb; l < r; l >>= 1, r >>= 1) {
            if(l & 1) res = op(res, data[id(hr, l++)]);
            if(r & 1) res = op(res, data[id(hr, --r)]);
          }
        }
      }
      return res;
    }

    S all_prod() { return data[1]; }
};

}  // namespace soryuusi
