// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/dynamic_bit_vector.hpp
#pragma once
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <utility>
#include <vector>
#include "../math/internal/internal_bit.hpp"
#include "internal/node_pool.hpp"
#include "internal/treap_base.hpp"

namespace soryuusi {

class DynamicBitVector {
  template<class, int> friend struct DynamicWavelet;
  using Word = std::uint64_t;
  static constexpr int word_bits = std::numeric_limits<Word>::digits;
  static constexpr int block_bits = word_bits/2;
  static constexpr int build_bits = 48;
  struct Node {
    Node* pp = nullptr;
    Node* lp = nullptr;
    Node* rp = nullptr;
    Word bits;
    int total_length, total_pop;
    std::uint32_t priority = 0;
    std::uint8_t length, pop;
    Node(Word value, int len) : bits(value), total_length(len), total_pop(soryuusi::popcount(value)), length(len), pop(total_pop){}
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
  internal::NodePool<Node> pool;
  std::vector<Node*> free_nodes;
  Node* root = nullptr;
  internal::TreapBase<Node,Ops> engine;
  static Word mask(int n){return (Word(1)<<n)-1;}
  Node* make_node(Word bits, int length){
    Node* p;
    if(free_nodes.empty()) p = pool.create(bits,length);
    else{p = free_nodes.back(); free_nodes.pop_back(); *p = Node(bits,length);}
    engine.prepare(p);
    return p;
  }
  static void add_to_path(Node* p, int length, int ones){
    for(;p;p = p->pp){p->total_length += length; p->total_pop += ones;}
  }
  template<bool AllowEnd, bool WithRank> Node* locate(int& i, int& prefix) const {
    for(Node* p = root;;){
      int left = Ops::size(p->lp);
      if(i < left){p = p->lp; continue;}
      i -= left;
      if(i < p->length || (AllowEnd && i == p->length && !p->rp)){
        if constexpr(WithRank) prefix += Ops::ones(p->lp)+soryuusi::popcount(p->bits & mask(i));
        return p;
      }
      if constexpr(WithRank) prefix += Ops::ones(p->lp)+p->pop;
      i -= p->length;
      p = p->rp;
    }
  }
  static int prefix_rank(const Node* p, int k){
    int prefix = 0;
    while(p){
      int left = Ops::size(p->lp);
      if(k < left){p = p->lp; continue;}
      prefix += Ops::ones(p->lp); k -= left;
      if(k <= p->length) return prefix+soryuusi::popcount(p->bits & mask(k));
      prefix += p->pop; k -= p->length; p = p->rp;
    }
    return prefix;
  }
  std::pair<int,int> rank_pair(int l, int r) const {
    int n = size(), ones = Ops::ones(root);
    assert(0 <= l && l <= r && r <= n);
    if(ones == 0) return {0,0};
    if(ones == n) return {l,r};
    if(l == r){int k = rank(l); return {k,k};}
    if(l == 0 || r == n) return {l == 0 ? 0 : prefix_rank(root,l),r == n ? ones : prefix_rank(root,r)};
    int prefix = 0;
    const Node* p = root;
    for(;;){
      int left = Ops::size(p->lp), end = left+p->length;
      if(r < left){p = p->lp; continue;}
      int before = Ops::ones(p->lp);
      if(l >= end){prefix += before+p->pop; l -= end; r -= end; p = p->rp; continue;}
      int low = l < left ? prefix_rank(p->lp,l) : before+soryuusi::popcount(p->bits & mask(l-left));
      int high = r > end ? before+p->pop+prefix_rank(p->rp,r-end) : before+soryuusi::popcount(p->bits & mask(r-left));
      return {prefix+low,prefix+high};
    }
  }
  template<class P> static P* first_node(P* p){
    if(p) while(p->lp) p = p->lp;
    return p;
  }
  template<class P> static P* next_node(P* p){
    if(p->rp) return first_node(p->rp);
    while(p->pp && p->pp->rp == p) p = p->pp;
    return p->pp;
  }
  template<class Pack> void build_blocks(int n, Pack pack){
    root = nullptr; free_nodes.clear(); pool = internal::NodePool<Node>();
    int blocks = n/build_bits+(n%build_bits != 0);
    root = engine.build(blocks,[&](int block){
      int start = block*build_bits, length = std::min(build_bits,n-start);
      return make_node(pack(start,length),length);
    });
  }
  std::pair<int,bool> get_and_rank(int i) const {
    int ones = Ops::ones(root);
    if(ones == 0) return {0,false};
    if(ones == size()) return {i,true};
    int prefix = 0;
    Node* p = locate<false,true>(i,prefix);
    return {prefix,(p->bits>>i)&1};
  }
  template<bool WithRank> std::pair<int,bool> set_impl(int i, bool value){
    assert(0 <= i && i < size());
    int prefix = 0;
    Node* p = locate<false,WithRank>(i,prefix);
    bool old = (p->bits>>i)&1;
    if(value != old){
      p->bits ^= Word(1)<<i;
      int delta = int(value)-int(old);
      p->pop += delta;
      add_to_path(p,0,delta);
    }
    return {prefix,old};
  }
  template<bool WithRank> int insert_impl(int i, bool value){
    assert(0 <= i && i <= size());
    if(!root){root = make_node(Word(value),1); return 0;}
    int prefix = 0;
    Node* p = locate<true,WithRank>(i,prefix);
    Word lower = mask(i);
    Word bits = (p->bits & lower) | ((p->bits & ~lower)<<1) | (Word(value)<<i);
    if(p->length+1 == word_bits){
      int cut = i == p->length ? build_bits : block_bits;
      Node* next = make_node(bits>>cut,word_bits-cut);
      p->bits = bits & mask(cut);
      p->length = cut; p->pop = soryuusi::popcount(p->bits);
      Node* parent = p;
      if(!p->rp) p->rp = next;
      else{parent = p->rp; while(parent->lp) parent = parent->lp; parent->lp = next;}
      next->pp = parent;
      // Rotations and the final upward pass repair both changed blocks.
      root = engine.bubble_up(next);
    }else{
      p->bits = bits; ++p->length; p->pop += value;
      add_to_path(p,1,value);
    }
    return prefix;
  }
  void erase_block(Node* p){
    free_nodes.push_back(p);
    Node* parent = p->pp;
    Node* replacement = engine.join(p->lp,p->rp);
    if(replacement) replacement->pp = parent;
    if(!parent) root = replacement;
    else if(parent->lp == p) parent->lp = replacement;
    else parent->rp = replacement;
    for(;parent;parent = parent->pp) Ops::pull(parent);
  }
  template<bool WithRank> std::pair<int,bool> erase_impl(int i){
    assert(0 <= i && i < size());
    int prefix = 0;
    Node* p = locate<false,WithRank>(i,prefix);
    bool old = (p->bits>>i)&1;
    if(p->length == 1) erase_block(p);
    else {
      Word bits = (p->bits & mask(i)) | ((p->bits>>(i+1))<<i);
      int length = p->length-1, ones = p->pop-old;
      if(length == block_bits/4){
        Node* next = next_node(p);
        if(next && length+next->length <= block_bits){
          erase_block(p);
          next->bits = (next->bits<<length)|bits;
          next->length += length; next->pop += ones;
          add_to_path(next,length,ones);
          return {prefix,old};
        }
      }
      p->bits = bits; p->length = length; p->pop = ones;
      add_to_path(p,-1,-int(old));
    }
    return {prefix,old};
  }

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
    build_blocks(values.size(),[&](int start, int length){
      Word bits = 0;
      for(int i = 0; i < length; ++i) bits |= Word(values[start+i])<<i;
      return bits;
    });
  }
  bool get(int i) const {
    assert(0 <= i && i < size());
    int prefix = 0;
    Node* p = locate<false,false>(i,prefix);
    return (p->bits>>i)&1;
  }
  void set(int i, bool value){set_impl<false>(i,value);}
  // Number of ones in [0,k).
  int rank(int k) const {
    assert(0 <= k && k <= size());
    if(k == 0) return 0;
    int n = size(), ones = Ops::ones(root);
    if(k == n || ones == 0) return ones;
    if(ones == n) return k;
    return prefix_rank(root,k);
  }
  int rank(int l, int r) const {auto [lo,hi] = rank_pair(l,r); return hi-lo;}
  void insert(int i, bool value){insert_impl<false>(i,value);}
  void erase(int i){erase_impl<false>(i);}
  std::vector<bool> to_vector() const {
    std::vector<bool> values;
    values.reserve(size());
    for(const Node* p = first_node(root); p; p = next_node(p)){
      for(int i = 0; i < p->length; ++i) values.push_back((p->bits>>i)&1);
    }
    return values;
  }
  [[nodiscard]] DynamicBitVector clone() const {
    DynamicBitVector result;
    const Node* p = first_node(root);
    int offset = 0;
    result.build_blocks(size(),[&](int, int length){
      Word bits = 0;
      for(int filled = 0; filled < length;){
        int take = std::min(length-filled,p->length-offset);
        bits |= ((p->bits>>offset)&mask(take))<<filled;
        filled += take; offset += take;
        if(offset == p->length){p = next_node(p); offset = 0;}
      }
      return bits;
    });
    return result;
  }
};

}  // namespace soryuusi
