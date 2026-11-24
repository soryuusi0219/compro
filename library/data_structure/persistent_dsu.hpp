// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/persistent_dsu.hpp
#pragma once
#include <cassert>
#include <cstddef>
#include <utility>
#include "persistent_vector.hpp"

namespace soryuusi {

class PersistentDSU {
  using Array = Persistent_Vector<int>;
  Array data;
  int n = -1;
public:
  using Root = Array::Root;

private:
  Root initial;

  std::pair<int,int> find_leader(Root r,int i) const {
    assert(0 <= i && i < n && r != Root{});
    const auto* root = data.pool.get(r);
    for(;;){
      int p = *Array::find_entry(root,i);
      if(p < 0) return {i,-p};
      i = p;
    }
  }
public:
  PersistentDSU() = default;
  explicit PersistentDSU(int m){initial = init(m);}
  [[nodiscard]] Root init(int m){
    assert(n == -1 && m >= 0);
    initial = data.build_uniform(m,-1);
    n = m;
    return initial;
  }
  Root empty_root() const {assert(n >= 0); return initial;}
  std::size_t allocated_nodes() const {return data.allocated_nodes();}
  int leader(Root r, int i) const {return find_leader(r,i).first;}
  int leader(int i, Root r) const {return leader(r,i);}
  bool same(Root r, int i, int j) const {return leader(r,i) == leader(r,j);}
  bool same(int i, int j, Root r) const {return same(r,i,j);}
  int size(Root r, int i) const {return find_leader(r,i).second;}
  int size(int i, Root r) const {return size(r,i);}
  [[nodiscard]] std::pair<int,Root> merge(int i, int j, Root r){
    auto [x,sx] = find_leader(r,i);
    auto [y,sy] = find_leader(r,j);
    if(x == y) return {x,r};
    if(sx < sy) std::swap(x,y);
    return {x,data.set_pair(r,x,-(sx+sy),y,x)};
  }
  [[nodiscard]] Root merge(Root r, int i, int j){return merge(i,j,r).second;}
};

}  // namespace soryuusi
