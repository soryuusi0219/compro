// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/persistent_dsu.hpp
#pragma once
#include <cassert>
#include <cstddef>
#include <utility>
#include <vector>
#include "library/data_structure/persistent_vector.hpp"

namespace soryuusi {

class PersistentDSU {
  using Array = Persistent_Vector<std::pair<int,int>>;
  Array data;
  int n = -1;
public:
  using Root = Array::Root;

private:
  Root initial;
public:
  PersistentDSU() = default;
  explicit PersistentDSU(int m){initial = init(m);}
  [[nodiscard]] Root init(int m){
    assert(n == -1 && m >= 0);
    n = m;
    initial = data.build(std::vector<std::pair<int,int>>(n,{-1,1}));
    return initial;
  }
  Root empty_root() const {assert(n >= 0); return initial;}
  std::size_t allocated_nodes() const {return data.allocated_nodes();}
  int leader(Root r, int i) const {
    assert(0 <= i && i < n && r != Root{});
    for(;;){auto p = data.get(r,i); if(p.first < 0) return i; i = p.first;}
  }
  int leader(int i, Root r) const {return leader(r,i);}
  bool same(Root r, int i, int j) const {return leader(r,i) == leader(r,j);}
  bool same(int i, int j, Root r) const {return same(r,i,j);}
  int size(Root r, int i) const {return data.get(r,leader(r,i)).second;}
  int size(int i, Root r) const {return size(r,i);}
  [[nodiscard]] std::pair<int,Root> merge(int i, int j, Root r){
    int x = leader(r,i), y = leader(r,j);
    if(x == y) return {x,r};
    int sx = size(r,x), sy = size(r,y);
    if(sx < sy) std::swap(x,y);
    auto next = data.set(r,x,{-1,sx+sy});
    next = data.set(next,y,{x,0});
    return {x,next};
  }
  [[nodiscard]] Root merge(Root r, int i, int j){return merge(i,j,r).second;}
};

}  // namespace soryuusi
