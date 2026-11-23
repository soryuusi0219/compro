// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/dynamic_bit_vector.hpp
#pragma once
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <utility>
#include <vector>
#include "../math/internal/internal_bit.hpp"
#include "internal/binary_tree_ops.hpp"
#include "internal/node_pool.hpp"
#include "internal/treap_base.hpp"

namespace soryuusi {

class DynamicBitVector {
  static constexpr int block_bits = 32;
  using Word = std::uint64_t;
  struct Node {
    Node* pp = nullptr;
    Node* lp = nullptr;
    Node* rp = nullptr;
    Word bits;
    int pop, total_pop, length, total_length;
    std::uint32_t priority = 0;
    Node(Word value, int len) : bits(value), pop(soryuusi::popcount(value)), total_pop(pop), length(len), total_length(len){}
  };
  struct Ops {
    static int size(const Node* p){return p ? p->total_length : 0;}
    static int ones(const Node* p){return p ? p->total_pop : 0;}
    static int local_size(const Node* p){return p->length;}
    static std::uint32_t& priority(Node* p){return p->priority;}
    static void push(Node*){}
    static void pull(Node* p){
      p->total_length = p->length+size(p->lp)+size(p->rp);
      p->total_pop = p->pop+ones(p->lp)+ones(p->rp);
    }
  };
  using Links = internal::BinaryTreeOps<Node,Ops>;
  internal::NodePool<Node> pool;
  std::vector<Node*> free_nodes;
  Node* root = nullptr;
  internal::TreapBase<Node,Ops> engine;
  static Word mask(int n){return n == 64 ? ~Word(0) : (Word(1)<<n)-1;}
  Node* make_node(Word bits, int length){
    Node* p;
    if(free_nodes.empty()) p = pool.create(bits,length);
    else{p = free_nodes.back(); free_nodes.pop_back(); *p = Node(bits,length);}
    engine.prepare(p);
    return p;
  }
  static void update_path(Node* p){for(;p;p = p->pp) Ops::pull(p);}
public:
  DynamicBitVector() = default;
  explicit DynamicBitVector(const std::vector<bool>& values){build(values);}
  DynamicBitVector(const DynamicBitVector&) = delete;
  DynamicBitVector& operator=(const DynamicBitVector&) = delete;
  DynamicBitVector(DynamicBitVector&& other) noexcept : pool(std::move(other.pool)), free_nodes(std::move(other.free_nodes)), root(std::exchange(other.root,nullptr)), engine(std::move(other.engine)){other.free_nodes.clear();}
  DynamicBitVector& operator=(DynamicBitVector&& other) noexcept {
    if(this != &other){
      pool = std::move(other.pool); free_nodes = std::move(other.free_nodes);
      root = std::exchange(other.root,nullptr); engine = std::move(other.engine); other.free_nodes.clear();
    }
    return *this;
  }
  int size() const {return Ops::size(root);}
  bool empty() const {return !root;}
  std::size_t allocated_nodes() const {return pool.size();}
  void build(const std::vector<bool>& values){
    root = nullptr; free_nodes.clear(); pool = internal::NodePool<Node>();
    int n = values.size();
    root = engine.build((n+block_bits-1)/block_bits,[&](int block){
      int start = block*block_bits, len = std::min(block_bits,n-start);
      Word bits = 0;
      for(int j = 0; j < len; ++j) if(values[start+j]) bits |= Word(1)<<j;
      return make_node(bits,len);
    });
  }
  bool get(int i) const {
    assert(0 <= i && i < size());
    auto [p,k] = Links::locate(root,i);
    return (p->bits>>k)&1;
  }
  void set(int i, bool value){
    assert(0 <= i && i < size());
    auto [p,k] = Links::locate(root,i);
    p->bits = (p->bits & ~(Word(1)<<k)) | (Word(value)<<k);
    p->pop = soryuusi::popcount(p->bits); update_path(p);
  }
  // Number of ones in [0,k).
  int rank(int k) const {
    assert(0 <= k && k <= size());
    int result = 0;
    for(Node* p = root; p;){
      int left = Ops::size(p->lp);
      if(k < left){p = p->lp; continue;}
      result += Ops::ones(p->lp); k -= left;
      if(k <= p->length) return result+soryuusi::popcount(p->bits & mask(k));
      k -= p->length; result += p->pop; p = p->rp;
    }
    return result;
  }
  int rank(int l, int r) const {assert(0 <= l && l <= r && r <= size()); return rank(r)-rank(l);}
  void insert(int i, bool value){
    assert(0 <= i && i <= size());
    if(!root){root = make_node(Word(value),1); return;}
    auto [p,k] = Links::locate(root,i,true);
    p->bits = (p->bits & mask(k)) | ((p->bits & ~mask(k))<<1) | (Word(value)<<k);
    ++p->length; p->pop = soryuusi::popcount(p->bits);
    if(p->length == 64){
      Node* next = make_node(p->bits>>block_bits,block_bits);
      p->bits &= mask(block_bits); p->length = block_bits; p->pop = soryuusi::popcount(p->bits);
      update_path(p);
      Node* parent = p;
      if(!p->rp) p->rp = next;
      else{parent = p->rp; while(parent->lp) parent = parent->lp; parent->lp = next;}
      next->pp = parent;
      update_path(parent);
      root = engine.bubble_up(next);
    }else update_path(p);
  }
  void erase(int i){
    assert(0 <= i && i < size());
    auto [p,k] = Links::locate(root,i);
    p->bits = (p->bits & mask(k)) | (k == 63 ? 0 : (p->bits>>(k+1))<<k);
    --p->length; p->pop = soryuusi::popcount(p->bits);
    if(p->length){update_path(p); return;}
    Node* parent = p->pp;
    Node* replacement = engine.join(p->lp,p->rp);
    if(replacement) replacement->pp = parent;
    if(!parent) root = replacement;
    else if(parent->lp == p) parent->lp = replacement;
    else parent->rp = replacement;
    free_nodes.push_back(p);
    update_path(parent);
  }
  std::vector<bool> to_vector() const {
    std::vector<bool> values;
    values.reserve(size());
    std::vector<Node*> stack;
    Node* p = root;
    while(p || !stack.empty()){
      while(p){stack.push_back(p); p = p->lp;}
      p = stack.back(); stack.pop_back();
      for(int i = 0; i < p->length; ++i) values.push_back((p->bits>>i)&1);
      p = p->rp;
    }
    return values;
  }
  [[nodiscard]] DynamicBitVector clone() const {return DynamicBitVector(to_vector());}
};

}  // namespace soryuusi
