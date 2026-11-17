// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/persistent_vector.hpp
#pragma once
#include <cassert>
#include <cstddef>
#include <vector>

#include "internal/node_pool.hpp"

namespace soryuusi {

template<class T>
class Persistent_Vector {
  struct node {
    T data{};
    node* c[20] = {};
  };
  internal::NodePool<node> pool;
  node* update(node* p, int i, const T& x){
    auto q = p ? pool.create(*p) : pool.create();
    if(i == 0) q->data = x;
    else q->c[i%20] = update(q->c[i%20],i/20,x);
    return q;
  }
  // Only used before publishing the result of build().
  void build_set(node*& p, int i, const T& x){
    if(!p) p = pool.create();
    if(i == 0) p->data = x;
    else build_set(p->c[i%20],i/20,x);
  }
public:
  using Root = typename internal::NodePool<node>::Root;

  Root empty_root() const {return pool.root(nullptr);}
  Root get_root() const {return empty_root();}
  std::size_t allocated_nodes() const {return pool.size();}
  [[nodiscard]] Root build(const std::vector<T>& a){
    node* p = nullptr;
    for(int i = 0; i < (int)a.size(); ++i) build_set(p,i,a[i]);
    return pool.root(p);
  }
  [[nodiscard]] Root set(Root r, int i, const T& x){assert(i >= 0); return pool.root(update(pool.get(r),i,x));}
  [[nodiscard]] Root set(int i, const T& x, Root r){return set(r,i,x);}
  T get(Root r, int i) const {
    assert(i >= 0);
    auto p = pool.get(r);
    while(p && i){p = p->c[i%20]; i /= 20;}
    return p ? p->data : T{};
  }
  T get(int i, Root r) const {return get(r,i);}
};

}  // namespace soryuusi
