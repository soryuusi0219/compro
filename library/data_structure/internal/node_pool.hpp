// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/internal/node_pool.hpp
#pragma once
#include <cassert>
#include <cstddef>
#include <deque>
#include <memory>
#include <utility>

namespace soryuusi::internal {

template<class Node> class NodePool;

// A borrowed version handle. Nodes and their owner are deliberately inaccessible.
template<class Node>
class NodeRoot {
  friend class NodePool<Node>;
  Node* ptr = nullptr;
#ifndef NDEBUG
  std::size_t domain = 0;
#endif
public:
  NodeRoot() = default;
  friend bool operator==(const NodeRoot&, const NodeRoot&) = default;
};

// deque keeps node addresses stable and allocates storage in blocks.
// No node is reclaimed individually. Moving the pool preserves its domain.
template<class Node>
class NodePool {
  struct Storage {
    std::deque<Node> nodes;
#ifndef NDEBUG
    inline static std::size_t next_domain = 0;
    std::size_t domain = ++next_domain;
#endif
  };
  std::unique_ptr<Storage> storage;
  Storage& ensure(){
    if(!storage) storage = std::make_unique<Storage>();
    return *storage;
  }
public:
  using Root = NodeRoot<Node>;
  NodePool() = default;
  NodePool(const NodePool&) = delete;
  NodePool& operator=(const NodePool&) = delete;
  NodePool(NodePool&&) noexcept = default;
  NodePool& operator=(NodePool&&) noexcept = default;
  template<class... Args> Node* create(Args&&... args){
    auto& nodes = ensure().nodes;
    nodes.emplace_back(std::forward<Args>(args)...);
    return &nodes.back();
  }
  std::size_t size() const {return storage ? storage->nodes.size() : 0;}
  Root root(Node* p) const {
    Root r;
    r.ptr = p;
#ifndef NDEBUG
    r.domain = storage ? storage->domain : 0;
#endif
    return r;
  }
  Node* get(Root r) const {
#ifndef NDEBUG
    assert(r.domain == 0 || (storage && r.domain == storage->domain));
#endif
    return r.ptr;
  }
};

}  // namespace soryuusi::internal
