// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/internal/sequence_base.hpp
#pragma once
#include <cassert>
#include <cstddef>
#include <iostream>
#include <memory>
#include <utility>
#include <vector>
#include "tree_arena.hpp"

namespace soryuusi::internal {

// Backend<Node,Ops>: prepare, access(root&,k), split, join, build(n,factory).
// Nodes have parent/left/right links; one node represents one sequence element.
template<class Ops, template<class,class> class Backend> class SequenceArena;

template<class Ops, template<class,class> class Backend>
class SequenceBase {
  using Node = typename Ops::Node;
  using T = typename Ops::value_type;
  friend class SequenceArena<Ops,Backend>;
  using Arena = TreeArena<Ops,Backend>;
  std::shared_ptr<Arena> arena;
  Node* root = nullptr;
  explicit SequenceBase(std::shared_ptr<Arena> owner, Node* p = nullptr) : arena(std::move(owner)), root(p){}
  Node* make_node(const T& value){assert(arena); return arena->create(value);}
  Node* build(const std::vector<T>& v){return arena->engine.build(v.size(),[&](int i){return make_node(v[i]);});}
  template<class Function> auto range(int l, int r, Function function){
    assert(0 <= l && l <= r && r <= size());
    if(l == r) return function(nullptr);
    auto [a,bc] = arena->engine.split(root,l);
    auto [b,c] = arena->engine.split(bc,r-l);
    auto result = function(b);
    root = arena->engine.join(arena->engine.join(a,b),c);
    return result;
  }
public:
  using value_type = T;
  SequenceBase(const SequenceBase&) = delete;
  SequenceBase& operator=(const SequenceBase&) = delete;
  SequenceBase(SequenceBase&& other) noexcept : arena(std::move(other.arena)), root(std::exchange(other.root,nullptr)){}
  SequenceBase& operator=(SequenceBase&& other) noexcept {
    if(this != &other){arena = std::move(other.arena); root = std::exchange(other.root,nullptr);}
    return *this;
  }
  int size() const {return Ops::size(root);}
  bool empty() const {return !root;}
  std::size_t allocated_nodes() const {return arena ? arena->pool.size() : 0;}
  void insert_at(int k, T value){
    assert(0 <= k && k <= size());
    auto [a,b] = arena->engine.split(root,k);
    root = arena->engine.join(arena->engine.join(a,make_node(value)),b);
  }
  void erase_at(int k){
    assert(0 <= k && k < size());
    auto [a,bc] = arena->engine.split(root,k);
    auto [b,c] = arena->engine.split(bc,1);
    arena->release(b);
    root = arena->engine.join(a,c);
  }
  T get(int k){assert(0 <= k && k < size()); return arena->engine.access(root,k)->value;}
  void set(int k, T value){
    assert(0 <= k && k < size());
    Node* p = arena->engine.access(root,k);
    p->value = value;
    for(;p;p = p->pp) Ops::pull(p);
  }
  void flip(int l, int r){range(l,r,[](Node* p){Ops::toggle(p); return 0;});}
  void flip_all(){Ops::toggle(root);}
  void apply(int l, int r, typename Ops::action_type f) requires Ops::has_action {
    range(l,r,[&](Node* p){Ops::apply(p,f); return 0;});
  }
  T prod(int l, int r){return range(l,r,[](Node* p){return p ? p->lsum : Ops::identity();});}
  T prodrev(int l, int r){return range(l,r,[](Node* p){return p ? p->rsum : Ops::identity();});}
  T prod_all() const {return root ? root->lsum : Ops::identity();}
  T prodrev_all() const {return root ? root->rsum : Ops::identity();}
  [[nodiscard]] std::pair<SequenceBase,SequenceBase> split(int k){
    assert(0 <= k && k <= size());
    assert(arena);
    SequenceBase a(arena),b(arena);
    auto [left,right] = arena->engine.split(root,k);
    a.root = left; b.root = right; root = nullptr;
    return {std::move(a),std::move(b)};
  }
  void concat(SequenceBase&& other){
    assert(this != &other);
    if(!other.root) return;
    assert(arena && other.arena);
    if(!root && arena == other.arena){root = std::exchange(other.root,nullptr); return;}
    Node* tail = other.root;
    if(arena != other.arena) tail = build(other.to_vector());
    root = arena->engine.join(root,tail);
    other.root = nullptr;
  }
  std::vector<T> to_vector(){
    std::vector<T> result;
    result.reserve(size());
    std::vector<Node*> stack;
    Node* p = root;
    while(p || !stack.empty()){
      while(p){Ops::push(p); stack.push_back(p); p = p->lp;}
      p = stack.back(); stack.pop_back(); result.push_back(p->value); p = p->rp;
    }
    return result;
  }
  [[nodiscard]] SequenceBase clone(){assert(arena); SequenceBase copy(arena); copy.root = copy.build(to_vector()); return copy;}
  void clear(){root = nullptr;}
  void print(){for(const auto& value : to_vector()) std::cout << value << ' '; std::cout << '\n';}
};

// Creates independent move-only sequences in one shared allocation domain.
template<class Ops, template<class,class> class Backend>
class SequenceArena {
  using Storage = TreeArena<Ops,Backend>;
  std::shared_ptr<Storage> arena = std::make_shared<Storage>();
  void ensure(){if(!arena) arena = std::make_shared<Storage>();}
public:
  using Sequence = SequenceBase<Ops,Backend>;
  SequenceArena() = default;
  SequenceArena(const SequenceArena&) = delete;
  SequenceArena& operator=(const SequenceArena&) = delete;
  SequenceArena(SequenceArena&&) noexcept = default;
  SequenceArena& operator=(SequenceArena&&) noexcept = default;
  [[nodiscard]] Sequence new_sequence(){ensure(); return Sequence(arena);}
  [[nodiscard]] Sequence new_sequence(const std::vector<typename Ops::value_type>& values){
    auto seq = new_sequence(); seq.root = seq.build(values); return seq;
  }
  std::size_t allocated_nodes() const {return arena ? arena->pool.size() : 0;}
};

}  // namespace soryuusi::internal
