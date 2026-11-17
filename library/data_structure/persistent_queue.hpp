// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/persistent_queue.hpp
#pragma once
#include <cassert>
#include <cstddef>
#include <vector>

#include "internal/node_pool.hpp"

namespace soryuusi {

template<class T>
class Persistent_Queue {
  struct node {
    T value;
    std::vector<node*> up;
    node(const T& x, node* prev) : value(x){
      if(prev) up.push_back(prev);
      for(int k = 1; !up.empty() && (int)up[k-1]->up.size() >= k; ++k)
        up.push_back(up[k-1]->up[k-1]);
    }
  };
  internal::NodePool<node> pool;
public:
  class Root {
    friend class Persistent_Queue;
    typename internal::NodePool<node>::Root back;
    int count = 0;
  public:
    Root() = default;
    friend bool operator==(const Root&, const Root&) = default;
  };

  Root empty_root() const {Root r; r.back = pool.root(nullptr); return r;}
  // An empty value, retained for source compatibility.
  inline static const Root origin{};
  std::size_t allocated_nodes() const {return pool.size();}
  int size(Root r) const {pool.get(r.back); return r.count;}
  bool empty(Root r) const {return size(r) == 0;}
  const T& front(Root r) const {
    assert(!empty(r));
    auto p = pool.get(r.back);
    int steps = r.count-1;
    for(int k = 0; steps; ++k, steps >>= 1) if(steps&1) p = p->up[k];
    return p->value;
  }
  [[nodiscard]] Root push(Root r, const T& x){
    auto p = pool.get(r.back);
    r.back = pool.root(pool.create(x,p));
    ++r.count;
    return r;
  }
  [[nodiscard]] Root push(const T& x, Root r){return push(r,x);}
  [[nodiscard]] Root pop(Root r) const {
    assert(!empty(r));
    --r.count;
    if(!r.count) r.back = pool.root(nullptr);
    return r;
  }
};

}  // namespace soryuusi
