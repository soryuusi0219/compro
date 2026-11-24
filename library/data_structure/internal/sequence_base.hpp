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
    if constexpr(requires {arena->engine.with_range(root,l,r,function);})
      return arena->engine.with_range(root,l,r,function);
    auto [a,bc] = arena->engine.split(root,l);
    auto [b,c] = arena->engine.split(bc,r-l);
    auto result = function(b);
    root = arena->engine.join(arena->engine.join(a,b),c);
    return result;
  }
public:
  using value_type = T;
  using NodeRef = typename Arena::NodeRef;
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
    arena->insert_at(root,k,make_node(value));
  }
  void erase_at(int k){
    assert(0 <= k && k < size());
    arena->erase_at(root,k);
  }
  T get(int k){assert(0 <= k && k < size()); return arena->engine.access(root,k)->value;}
  [[nodiscard]] NodeRef ref_at(int k){
    assert(0 <= k && k < size());
    return arena->reference(arena,arena->engine.access(root,k));
  }
  int index_of(const NodeRef& ref){return arena ? arena->index_of(root,ref) : -1;}
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
  T prod(int l, int r){
    assert(0 <= l && l <= r && r <= size());
    if(l == r) return Ops::identity();
    if(r-l == 1) return arena->engine.access(root,l)->value;
    return arena->template prod<false>(root,l,r);
  }
  T prodrev(int l, int r){
    assert(0 <= l && l <= r && r <= size());
    if(l == r) return Ops::identity();
    if(r-l == 1) return arena->engine.access(root,l)->value;
    return arena->template prod<true>(root,l,r);
  }
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
    if(arena != other.arena){
      std::vector<Node*> source;
      source.reserve(other.size());
      Arena::visit_in_order(other.root,[&](Node* p){source.push_back(p);});
      tail = arena->engine.build(source.size(),[&](int i){
        Node* p = make_node(source[i]->value);
        other.arena->transfer_reference(source[i],arena,p);
        return p;
      });
    }
    root = arena->engine.join(root,tail);
    other.root = nullptr;
  }
  std::vector<T> to_vector(){
    std::vector<T> result;
    result.reserve(size());
    Arena::visit_in_order(root,[&](Node* p){result.push_back(p->value);});
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
  using NodeRef = typename Sequence::NodeRef;
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
