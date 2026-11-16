// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/internal/sequence_base.hpp
#pragma once
#include <cassert>
#include <cstddef>
#include <iostream>
#include <memory>
#include <utility>
#include <type_traits>
#include <vector>
#include "library/data_structure/internal/node_pool.hpp"

namespace soryuusi::internal {

// Backend<Node,Ops>: prepare, access(root&,k), split, join, build(n,factory).
// Nodes have parent/left/right links; one node represents one sequence element.
template<class Ops, template<class,class> class Backend>
class SequenceBase {
  using Node = typename Ops::Node;
  using T = typename Ops::value_type;
  struct Arena {NodePool<Node> pool; std::vector<Node*> free_nodes;};
  std::shared_ptr<Arena> arena;
  Node* root = nullptr;
  Backend<Node,Ops> engine;
  Node* make_node(const T& value){
    if(!arena) arena = std::make_shared<Arena>();
    Node* p;
    if(arena->free_nodes.empty()) p = arena->pool.create(value);
    else{p = arena->free_nodes.back(); arena->free_nodes.pop_back(); *p = Node(value);}
    engine.prepare(p);
    return p;
  }
  Node* build(const std::vector<T>& v){return engine.build(v.size(),[&](int i){return make_node(v[i]);});}
  template<class Function> auto range(int l, int r, Function function){
    assert(0 <= l && l <= r && r <= size());
    if(l == r) return function(nullptr);
    auto [a,bc] = engine.split(root,l);
    auto [b,c] = engine.split(bc,r-l);
    auto result = function(b);
    root = engine.join(engine.join(a,b),c);
    return result;
  }
public:
  using value_type = T;
  SequenceBase() = default;
  explicit SequenceBase(const std::vector<T>& v){root = build(v);}
  SequenceBase(const SequenceBase&) = delete;
  SequenceBase& operator=(const SequenceBase&) = delete;
  SequenceBase(SequenceBase&& other) noexcept(std::is_nothrow_move_constructible_v<Backend<Node,Ops>>) : arena(std::move(other.arena)), root(std::exchange(other.root,nullptr)), engine(std::move(other.engine)){}
  SequenceBase& operator=(SequenceBase&& other) noexcept(std::is_nothrow_move_assignable_v<Backend<Node,Ops>>) {
    if(this != &other){arena = std::move(other.arena); root = std::exchange(other.root,nullptr); engine = std::move(other.engine);}
    return *this;
  }
  int size() const {return Ops::size(root);}
  bool empty() const {return !root;}
  std::size_t allocated_nodes() const {return arena ? arena->pool.size() : 0;}
  void insert_at(int k, T value){
    assert(0 <= k && k <= size());
    auto [a,b] = engine.split(root,k);
    root = engine.join(engine.join(a,make_node(value)),b);
  }
  void erase_at(int k){
    assert(0 <= k && k < size());
    auto [a,bc] = engine.split(root,k);
    auto [b,c] = engine.split(bc,1);
    arena->free_nodes.push_back(b);
    root = engine.join(a,c);
  }
  T get(int k){assert(0 <= k && k < size()); return engine.access(root,k)->value;}
  void set(int k, T value){
    assert(0 <= k && k < size());
    Node* p = engine.access(root,k);
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
    SequenceBase a,b;
    a.arena = b.arena = arena;
    auto [left,right] = engine.split(root,k);
    a.root = left; b.root = right; root = nullptr;
    return {std::move(a),std::move(b)};
  }
  void concat(SequenceBase&& other){
    assert(this != &other);
    if(!other.root) return;
    if(!root){arena = other.arena; root = std::exchange(other.root,nullptr); return;}
    Node* tail = other.root;
    if(arena != other.arena) tail = build(other.to_vector());
    root = engine.join(root,tail);
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
  [[nodiscard]] SequenceBase clone(){return SequenceBase(to_vector());}
  void clear(){root = nullptr;}
  void print(){for(const auto& value : to_vector()) std::cout << value << ' '; std::cout << '\n';}
};

}  // namespace soryuusi::internal
