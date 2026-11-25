// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/persistent_vector.hpp
#pragma once
#include <algorithm>
#include <array>
#include <cassert>
#include <cstddef>
#include <type_traits>
#include <variant>
#include <vector>

#include "../math/internal/internal_bit.hpp"
#include "internal/node_pool.hpp"

namespace soryuusi {

template<class T>
class Persistent_Vector {
  friend class PersistentDSU;
  template<class S, auto op> friend class PersistentDSUWithData;

  static constexpr unsigned radix = 8;
  static constexpr int radix_bits = soryuusi::countr_zero(radix);
  static constexpr unsigned leaf_capacity = std::is_trivially_copyable_v<T> ? soryuusi::bit_floor(static_cast<unsigned>(std::max<std::size_t>(1,radix*sizeof(void*)/sizeof(T)))) : 1;
  static constexpr int leaf_bits = soryuusi::countr_zero(leaf_capacity);

  struct node;
  using Children = std::array<node*,radix>;
  using Values = std::array<T,leaf_capacity>;
  struct node {
    int height;
    std::variant<Children,Values> storage;
    explicit node(int h) : height(h) {if(h == 0) storage.template emplace<Values>();}
    Children& children(){return *std::get_if<Children>(&storage);}
    const Children& children() const {return *std::get_if<Children>(&storage);}
    Values& values(){return *std::get_if<Values>(&storage);}
    const Values& values() const {return *std::get_if<Values>(&storage);}
  };
  internal::NodePool<node> pool;

  static int shift(int height){return leaf_bits+(height-1)*radix_bits;}
  static int child(int i,int height){return (static_cast<unsigned>(i) >> shift(height)) & (radix-1);}
  static int required_height(int i){
    int remaining = soryuusi::bit_width(static_cast<unsigned>(i))-leaf_bits;
    return remaining > 0 ? (remaining+radix_bits-1)/radix_bits : 0;
  }

  node* update(const node* p,int h,int i,const T& value){
    node* q = p ? pool.create(*p) : pool.create(h);
    if(h == 0) q->values()[i & (leaf_capacity-1)] = value;
    else {
      auto& branch = q->children()[child(i,h)];
      branch = update(branch,h-1,i,value);
    }
    return q;
  }
  node* update_pair(const node* p,int h,int i,const T& x,int j,const T& y){
    auto* q = p ? pool.create(*p) : pool.create(h);
    if(h == 0){
      q->values()[i & (leaf_capacity-1)] = x;
      q->values()[j & (leaf_capacity-1)] = y;
    } else {
      int a = child(i,h), b = child(j,h);
      auto& children = q->children();
      if(a == b) children[a] = update_pair(children[a],h-1,i,x,j,y);
      else {
        children[a] = update(children[a],h-1,i,x);
        children[b] = update(children[b],h-1,j,y);
      }
    }
    return q;
  }
  node* lift(node* p){
    auto* q = pool.create(p->height+1);
    q->children()[0] = p;
    return q;
  }
  node* build_range(std::size_t l,std::size_t r,int height,auto& value_at){
    node* q = pool.create(height);
    if(height == 0){
      for(std::size_t i = l; i < r; ++i) q->values()[i-l] = value_at(i);
    } else {
      const auto width = std::size_t{1} << shift(height);
      for(unsigned i = 0; l < r; ++i,l += width) {
        q->children()[i] = build_range(l,std::min(l+width,r),height-1,value_at);
      }
    }
    return q;
  }
public:
  using Root = typename internal::NodePool<node>::Root;

private:
  [[nodiscard]] Root build_uniform(int n,const T& value){
    if(n == 0) return empty_root();
    auto value_at = [&](std::size_t) -> const T& {return value;};
    return pool.root(build_range(0,n,required_height(n-1),value_at));
  }
  [[nodiscard]] Root set_pair(Root r,int i,const T& x,int j,const T& y){
    const auto* p = pool.get(r);
    assert(p && i >= 0 && j >= 0 && required_height(std::max(i,j)) <= p->height);
    return pool.root(update_pair(p,p->height,i,x,j,y));
  }
  const T* find(Root r,int i) const {
    assert(i >= 0);
    const node* p = pool.get(r);
    if(!p || required_height(i) > p->height) return nullptr;
    return find_entry(p,i);
  }
  // Used by the DSUs for indices inside their fully built initial range.
  static const T* find_entry(const node* p,int i){
    for(int h = p->height; p && h; --h) p = p->children()[child(i,h)];
    return p ? &p->values()[i & (leaf_capacity-1)] : nullptr;
  }

public:
  Root empty_root() const {return pool.root(nullptr);}
  Root get_root() const {return empty_root();}
  std::size_t allocated_nodes() const {return pool.size();}
  [[nodiscard]] Root build(const std::vector<T>& a){
    if(a.empty()) return empty_root();
    auto value_at = [&](std::size_t i) -> decltype(auto) {return a[i];};
    return pool.root(build_range(0,a.size(),required_height(static_cast<int>(a.size()-1)),value_at));
  }
  [[nodiscard]] Root set(Root r,int i,const T& value){
    assert(i >= 0);
    node* p = pool.get(r);
    int height = required_height(i);
    if(!p) return pool.root(update(nullptr,height,i,value));
    if(height <= p->height) return pool.root(update(p,p->height,i,value));
    while(p->height+1 < height) p = lift(p);
    node* q = lift(p);
    q->children()[child(i,height)] = update(nullptr,height-1,i,value);
    return pool.root(q);
  }
  [[nodiscard]] Root set(int i,const T& value,Root r){return set(r,i,value);}
  T get(Root r,int i) const {const T* p = find(r,i); return p ? *p : T{};}
  T get(int i,Root r) const {return get(r,i);}
};

}  // namespace soryuusi
