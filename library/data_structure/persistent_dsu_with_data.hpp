// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/persistent_dsu_with_data.hpp
#pragma once
#include <cassert>
#include <cstddef>
#include <type_traits>
#include <utility>
#include <vector>
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
public:
  PersistentDSUWithData() = default;
  PersistentDSUWithData(int m, S x){initial = init(m,x);}
  [[nodiscard]] Root init(int m, S x){
    assert(n == -1 && m >= 0);
    n = m;
    initial = data.build(std::vector<Entry>(n,Entry{-1,1,x}));
    return initial;
  }
  Root empty_root() const {assert(n >= 0); return initial;}
  std::size_t allocated_nodes() const {return data.allocated_nodes();}
  int leader(Root r, int i) const {
    assert(0 <= i && i < n && r != Root{});
    for(;;){int p = data.get(r,i).parent; if(p < 0) return i; i = p;}
  }
  int leader(int i, Root r) const {return leader(r,i);}
  bool same(Root r, int i, int j) const {return leader(r,i) == leader(r,j);}
  bool same(int i, int j, Root r) const {return same(r,i,j);}
  int size(Root r, int i) const {return data.get(r,leader(r,i)).size;}
  int size(int i, Root r) const {return size(r,i);}
  S get(Root r, int i) const {return data.get(r,leader(r,i)).value;}
  S get(int i, Root r) const {return get(r,i);}
  [[nodiscard]] std::pair<int,Root> merge(int i, int j, Root r){
    int x = leader(r,i), y = leader(r,j);
    if(x == y) return {x,r};
    auto a = data.get(r,x), b = data.get(r,y);
    S value = op(a.value,b.value);
    if(a.size < b.size) {std::swap(x,y); std::swap(a,b);}
    auto next = data.set(r,x,Entry{-1,a.size+b.size,value});
    next = data.set(next,y,Entry{x,0,S{}});
    return {x,next};
  }
  [[nodiscard]] Root merge(Root r, int i, int j){return merge(i,j,r).second;}
  [[nodiscard]] Root set(Root r, int i, S value){
    int x = leader(r,i);
    auto entry = data.get(r,x);
    entry.value = value;
    return data.set(r,x,entry);
  }
  [[nodiscard]] Root set(int i, S value, Root r){return set(r,i,value);}
};
}  // namespace soryuusi
