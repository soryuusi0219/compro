// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/persistent_dsu_with_data.hpp
#pragma once
#include <cassert>
#include <cstddef>
#include <type_traits>
#include <utility>
#include "persistent_vector.hpp"

namespace soryuusi {

template<class S, auto op>
class PersistentDSUWithData {
  static_assert(std::is_invocable_r_v<S,decltype(op),S,S>);
  struct Entry {int parent = -1, size = 1; S value{};};
  using Array = Persistent_Vector<Entry>;
  Array data;
  int n = -1;
public:
  using Root = typename Array::Root;

private:
  Root initial;

  std::pair<int,const Entry*> find_leader(Root r,int i) const {
    assert(0 <= i && i < n && r != Root{});
    const auto* root = data.pool.get(r);
    for(;;){
      const Entry* p = Array::find_entry(root,i);
      if(p->parent < 0) return {i,p};
      i = p->parent;
    }
  }
public:
  PersistentDSUWithData() = default;
  PersistentDSUWithData(int m, S x){initial = init(m,x);}
  [[nodiscard]] Root init(int m, S x){
    assert(n == -1 && m >= 0);
    initial = data.build_uniform(m,Entry{-1,1,x});
    n = m;
    return initial;
  }
  Root empty_root() const {assert(n >= 0); return initial;}
  std::size_t allocated_nodes() const {return data.allocated_nodes();}
  int leader(Root r, int i) const {return find_leader(r,i).first;}
  int leader(int i, Root r) const {return leader(r,i);}
  bool same(Root r, int i, int j) const {return leader(r,i) == leader(r,j);}
  bool same(int i, int j, Root r) const {return same(r,i,j);}
  int size(Root r, int i) const {return find_leader(r,i).second->size;}
  int size(int i, Root r) const {return size(r,i);}
  S get(Root r, int i) const {return find_leader(r,i).second->value;}
  S get(int i, Root r) const {return get(r,i);}
  [[nodiscard]] std::pair<int,Root> merge(int i, int j, Root r){
    auto [x,a] = find_leader(r,i);
    auto [y,b] = find_leader(r,j);
    if(x == y) return {x,r};
    S left = a->value, right = b->value;
    S value = op(left,right);
    if(a->size < b->size) std::swap(x,y);
    return {x,data.set_pair(r,x,Entry{-1,a->size+b->size,value},y,Entry{x,0,S{}})};
  }
  [[nodiscard]] Root merge(Root r, int i, int j){return merge(i,j,r).second;}
  [[nodiscard]] Root set(Root r, int i, S value){
    auto [x,entry] = find_leader(r,i);
    return data.set(r,x,Entry{-1,entry->size,value});
  }
  [[nodiscard]] Root set(int i, S value, Root r){return set(r,i,value);}
};
}  // namespace soryuusi
