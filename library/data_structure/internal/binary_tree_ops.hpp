// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/internal/binary_tree_ops.hpp
#pragma once
#include <cassert>
#include <utility>

namespace soryuusi::internal {

// Intrusive, non-owning primitives. Ops supplies push/pull and logical sizes.
template<class Node, class Ops>
struct BinaryTreeOps {
  static bool is_root(Node* p){return !p->pp || (p->pp->lp != p && p->pp->rp != p);}
  // Ancestor tags must already be pushed. A path-parent is preserved.
  static void rotate(Node* p){
    Node* q = p->pp;
    Node* r = q->pp;
    if(q->lp == p){
      q->lp = p->rp;
      if(q->lp) q->lp->pp = q;
      p->rp = q;
    }else{
      assert(q->rp == p);
      q->rp = p->lp;
      if(q->rp) q->rp->pp = q;
      p->lp = q;
    }
    q->pp = p;
    p->pp = r;
    if(r){
      if(r->lp == q) r->lp = p;
      else if(r->rp == q) r->rp = p;
    }
    Ops::pull(q);
    Ops::pull(p);
  }
  // End insertion is supported. The result includes an offset within a block.
  static std::pair<Node*,int> locate(Node* p, int k, bool allow_end = false){
    assert(p && 0 <= k && (k < Ops::size(p) || (allow_end && k == Ops::size(p))));
    while(p){
      Ops::push(p);
      int left = Ops::size(p->lp), own = Ops::local_size(p);
      if(k < left) p = p->lp;
      else if(k < left+own || (allow_end && !p->rp && k == left+own)) return {p,k-left};
      else{k -= left+own; p = p->rp;}
    }
    assert(false);
    return {nullptr,0};
  }
};

}  // namespace soryuusi::internal
