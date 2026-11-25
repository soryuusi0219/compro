// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/skew_heap.hpp
#pragma once
#include <cassert>
#include <cstddef>
#include <memory>
#include <utility>
#include <variant>

#include "internal/node_pool.hpp"

namespace soryuusi {

namespace internal {
template<class Key, class Value>
struct SkewHeapNode {
  SkewHeapNode *left = nullptr, *right = nullptr;
  Key key, lazy{};
  [[no_unique_address]] Value value;

  void add(const Key& delta){key += delta; lazy += delta;}
};
}  // namespace internal

template<class Key, class Value = std::monostate>
class SkewHeap {
  using Node = internal::SkewHeapNode<Key,Value>;
  using Pool = internal::NodePool<Node>;
  std::shared_ptr<Pool> pool = std::make_shared<Pool>();

public:
  class Heap {
    friend class SkewHeap;
    std::shared_ptr<Pool> pool;
    Node* root = nullptr;
    std::size_t count = 0;

    explicit Heap(std::shared_ptr<Pool> p) : pool(std::move(p)) {}
    static void push_down(Node* p){
      if(p->lazy == Key{}) return;
      if(p->left) p->left->add(p->lazy);
      if(p->right) p->right->add(p->lazy);
      p->lazy = Key{};
    }
    static Node* meld_nodes(Node* a, Node* b){
      Node* result;
      Node** next = &result;
      while(a && b){
        if(b->key < a->key) std::swap(a,b);
        push_down(a);
        std::swap(a->left,a->right);
        *next = a;
        next = &a->right;
        a = a->right;
      }
      *next = a ? a : b;
      return result;
    }

  public:
    Heap(const Heap&) = delete;
    Heap& operator=(const Heap&) = delete;
    Heap(Heap&& other) noexcept
      : pool(std::move(other.pool)), root(std::exchange(other.root,nullptr)),
        count(std::exchange(other.count,0)) {}
    Heap& operator=(Heap&& other) noexcept {
      if(this == &other) return *this;
      pool = std::move(other.pool);
      root = std::exchange(other.root,nullptr);
      count = std::exchange(other.count,0);
      return *this;
    }
    bool empty() const {return count == 0;}
    std::size_t size() const {return count;}
    std::pair<Key,Value> top() const {assert(root); return {root->key,root->value};}
    void push(Key key, Value value = {}){
      assert(pool);
      root = meld_nodes(root,pool->create(nullptr,nullptr,std::move(key),Key{},std::move(value)));
      ++count;
    }
    void pop(){
      assert(root);
      push_down(root);
      root = meld_nodes(root->left,root->right);
      --count;
    }
    void add_all(const Key& delta){if(root) root->add(delta);}
    void meld(Heap&& other){
      assert(this != &other && pool && pool == other.pool);
      root = meld_nodes(root,std::exchange(other.root,nullptr));
      count += std::exchange(other.count,0);
    }
  };

  SkewHeap() = default;
  SkewHeap(const SkewHeap&) = delete;
  SkewHeap& operator=(const SkewHeap&) = delete;
  SkewHeap(SkewHeap&&) noexcept = default;
  SkewHeap& operator=(SkewHeap&&) noexcept = default;
  [[nodiscard]] Heap new_heap() const {assert(pool); return Heap(pool);}
  std::size_t allocated_nodes() const {return pool ? pool->size() : 0;}
};

template<class Key, class Value = std::monostate>
class PersistentSkewHeap {
  using Node = internal::SkewHeapNode<Key,Value>;
  using Pool = internal::NodePool<Node>;
  Pool pool;

  struct Branch {
    Node* node;
    Key offset;
  };
  Node* add_copy(Branch branch){
    if(!branch.node || branch.offset == Key{}) return branch.node;
    Node* copy = pool.create(*branch.node);
    copy->add(branch.offset);
    return copy;
  }
  Node* meld_nodes(Branch a, Branch b){
    Node* result;
    Node** next = &result;
    while(a.node && b.node){
      Key key = a.node->key + a.offset;
      Key other = b.node->key + b.offset;
      if(other < key){std::swap(a,b); key = std::move(other);}
      const Node& p = *a.node;
      Key pending = p.lazy + a.offset;
      Node* left = add_copy({p.right,pending});
      Node* copy = pool.create(left,nullptr,std::move(key),Key{},p.value);
      *next = copy;
      next = &copy->right;
      a = {p.left,pending};
    }
    *next = add_copy(a.node ? a : b);
    return result;
  }

public:
  class Root {
    friend class PersistentSkewHeap;
    typename Pool::Root root;
    std::size_t count = 0;
    Root(typename Pool::Root p, std::size_t n) : root(p), count(n) {}
  public:
    Root() = default;
    friend bool operator==(const Root&,const Root&) = default;
  };

  Root empty_root() const {return Root(pool.root(nullptr),0);}
  std::size_t allocated_nodes() const {return pool.size();}
  bool empty(Root p) const {return !pool.get(p.root);}
  std::size_t size(Root p) const {pool.get(p.root); return p.count;}
  std::pair<Key,Value> top(Root p) const {
    const Node* node = pool.get(p.root);
    assert(node);
    return {node->key,node->value};
  }
  [[nodiscard]] Root push(Root p, Key key, Value value = {}){
    Node* old = pool.get(p.root);
    if(!old || key < old->key) return Root(pool.root(pool.create(nullptr,old,std::move(key),Key{},std::move(value))),p.count+1);
    Node* node = pool.create(nullptr,nullptr,std::move(key),Key{},std::move(value));
    return Root(pool.root(meld_nodes({old,Key{}},{node,Key{}})),p.count+1);
  }
  [[nodiscard]] Root pop(Root p){
    const Node* node = pool.get(p.root);
    assert(node);
    return Root(pool.root(meld_nodes({node->left,node->lazy},{node->right,node->lazy})),p.count-1);
  }
  [[nodiscard]] Root add_all(Root p, const Key& delta){
    Node* node = pool.get(p.root);
    return Root(pool.root(add_copy({node,delta})),p.count);
  }
  [[nodiscard]] Root meld(Root a, Root b){
    return Root(pool.root(meld_nodes({pool.get(a.root),Key{}},{pool.get(b.root),Key{}})),a.count+b.count);
  }
};

}  // namespace soryuusi
