// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/internal/treap_base.hpp
#pragma once
#include <cassert>
#include <chrono>
#include <cstdint>
#include <utility>
#include "library/data_structure/internal/binary_tree_ops.hpp"
#include "library/data_structure/internal/cartesian_tree_base.hpp"

namespace soryuusi::internal {

struct TreapPriority {std::uint32_t priority = 0;};

// Parented implicit treap. Ops::size/local_size may measure blocks of elements.
template<class Node, class Ops>
class TreapBase {
  using Links = BinaryTreeOps<Node,Ops>;
  std::uint64_t state;
  Node* merge_nodes(Node* l, Node* r){
    if(!l || !r) return l ? l : r;
    if(Ops::priority(l) >= Ops::priority(r)){
      Ops::push(l); l->rp = merge_nodes(l->rp,r); l->rp->pp = l;
      Ops::pull(l); return l;
    }
    Ops::push(r); r->lp = merge_nodes(l,r->lp); r->lp->pp = r;
    Ops::pull(r); return r;
  }
  std::pair<Node*,Node*> split_nodes(Node* p, int k){
    if(!p) return {nullptr,nullptr};
    Ops::push(p);
    int left = Ops::size(p->lp), own = Ops::local_size(p);
    if(k <= left){
      auto [l,r] = split_nodes(p->lp,k);
      p->lp = r; if(r) r->pp = p;
      p->pp = nullptr; if(l) l->pp = nullptr;
      Ops::pull(p); return {l,p};
    }
    assert(k >= left+own); // Block interiors require a payload-specific split.
    auto [l,r] = split_nodes(p->rp,k-left-own);
    p->rp = l; if(l) l->pp = p;
    p->pp = nullptr; if(r) r->pp = nullptr;
    Ops::pull(p); return {p,r};
  }
public:
  explicit TreapBase(std::uint64_t seed = std::chrono::steady_clock::now().time_since_epoch().count()) : state(seed ? seed : 1){}
  void prepare(Node* p){state ^= state << 7; state ^= state >> 9; Ops::priority(p) = std::uint32_t(state >> 16);}
  Node* access(Node*& root, int k){return Links::locate(root,k).first;}
  Node* join(Node* l, Node* r){auto p = merge_nodes(l,r); if(p) p->pp = nullptr; return p;}
  std::pair<Node*,Node*> split(Node* p, int k){assert(0 <= k && k <= Ops::size(p)); return split_nodes(p,k);}
  // Restore the heap after attaching a newly allocated block at an inorder gap.
  Node* bubble_up(Node* p){
    while(p->pp && Ops::priority(p) > Ops::priority(p->pp)) Links::rotate(p);
    Node* root = p;
    for(Node* q = p->pp; q; q = q->pp){Ops::pull(q); root = q;}
    return root;
  }
  template<class Factory> Node* build(int n, Factory at){
    return build_cartesian<Node*>(n,nullptr,at,
      [](Node* a, Node* b){return Ops::priority(a) > Ops::priority(b);},
      [](Node* p, Node* l){p->lp = l; if(l) l->pp = p;},
      [](Node* p, Node* r){p->rp = r; if(r) r->pp = p;},
      [](Node* p){Ops::pull(p);});
  }
};

}  // namespace soryuusi::internal
