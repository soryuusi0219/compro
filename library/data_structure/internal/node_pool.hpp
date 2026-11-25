// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/internal/node_pool.hpp
#pragma once
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <memory>
#include <type_traits>
#include <utility>
#include <vector>

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

// Blocks grow from eight to 1024 slots and keep node addresses stable.
// No node is reclaimed individually. Moving the pool preserves its domain.
template<class Node>
class NodePool {
  struct Block {
    Node* data;
    std::size_t capacity;
    explicit Block(std::size_t n) : data(std::allocator<Node>{}.allocate(n)), capacity(n) {}
    Block(Block&& old) noexcept : data(std::exchange(old.data,nullptr)), capacity(old.capacity) {}
    ~Block(){if(data) std::allocator<Node>{}.deallocate(data,capacity);}
  };
  struct Storage {
    std::vector<Block> blocks;
    Node* next = nullptr;
    Node* end = nullptr;
    std::size_t count = 0;

    void append(){
      auto capacity = blocks.empty() ? 8 : std::min<std::size_t>(blocks.back().capacity*2,1024);
      auto& block = blocks.emplace_back(capacity);
      next = block.data;
      end = block.data+capacity;
    }

    ~Storage(){
      if constexpr(!std::is_trivially_destructible_v<Node>){
        if(blocks.empty()) return;
        for(std::size_t i = 0; i+1 < blocks.size(); ++i) {
          std::destroy_n(blocks[i].data,blocks[i].capacity);
        }
        Node* last = blocks.back().data;
        std::destroy_n(last,next-last);
      }
    }
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
    auto& s = ensure();
    if(s.next == s.end) s.append();
    auto* node = std::construct_at(s.next,std::forward<Args>(args)...);
    ++s.next;
    ++s.count;
    return node;
  }
  std::size_t size() const {return storage ? storage->count : 0;}
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
