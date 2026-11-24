// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/internal/treap_base.hpp
#pragma once
#include <algorithm>
#include <cassert>
#include <chrono>
#include <cstdint>
#include <utility>
#include "binary_tree_ops.hpp"
#include "cartesian_tree_base.hpp"

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
  // Result roots may retain old parents until the public split detaches them.
  std::pair<Node*,Node*> split_nodes(Node* p, int k){
    Ops::push(p);
    int left = Ops::size(p->lp), end = left+Ops::local_size(p);
    if(k == left){
      Node* detached = std::exchange(p->lp,nullptr);
      Ops::pull(p);
      return {detached,p};
    }
    if(k == end){
      Node* detached = std::exchange(p->rp,nullptr);
      Ops::pull(p);
      return {p,detached};
    }
    if(k < left){
      auto [l,r] = split_nodes(p->lp,k);
      p->lp = r;
      if(r) r->pp = p;
      Ops::pull(p);
      return {l,p};
    }
    assert(k > end); // Block interiors require a payload-specific split.
    auto [l,r] = split_nodes(p->rp,k-end);
    p->rp = l;
    if(l) l->pp = p;
    Ops::pull(p);
    return {p,r};
  }
  struct BlockSplit {Node* left; Node* right; Node* block; int offset;};
  BlockSplit split_block(Node* p, int k){
    Ops::push(p);
    p->pp = nullptr;
    int left = Ops::size(p->lp), end = left+Ops::local_size(p);
    if(k < left){
      auto parts = split_block(p->lp,k);
      p->lp = parts.right;
      if(p->lp) p->lp->pp = p;
      Ops::pull(p);
      parts.right = p;
      return parts;
    }
    if(k > end){
      auto parts = split_block(p->rp,k-end);
      p->rp = parts.left;
      if(p->rp) p->rp->pp = p;
      Ops::pull(p);
      parts.left = p;
      return parts;
    }
    Node* pred = nullptr;
    Node* succ = nullptr;
    if(k < end){
      pred = std::exchange(p->lp,nullptr);
      if(pred) pred->pp = nullptr;
    }
    if(k > left){
      succ = std::exchange(p->rp,nullptr);
      if(succ) succ->pp = nullptr;
    }
    if(k == left){Ops::pull(p); return {pred,p,nullptr,0};}
    if(k == end){Ops::pull(p); return {p,succ,nullptr,0};}
    return {pred,succ,p,k-left};
  }
  Node* insert_node(Node* tree, int k, Node* node){
    if(!tree) return node;
    if(Ops::priority(node) > Ops::priority(tree)){
      auto [left,right] = split(tree,k);
      node->lp = left;
      node->rp = right;
      if(left) left->pp = node;
      if(right) right->pp = node;
      Ops::pull(node);
      return node;
    }
    Ops::push(tree);
    int left = Ops::size(tree->lp);
    Node** branch = &tree->lp;
    if(k > left){
      k -= left+Ops::local_size(tree);
      assert(k >= 0);
      branch = &tree->rp;
    }
    *branch = insert_node(*branch,k,node);
    (*branch)->pp = tree;
    Ops::pull(tree);
    return tree;
  }
  Node* erase_node(Node* tree, int k, Node*& removed){
    Ops::push(tree);
    int left = Ops::size(tree->lp), own = Ops::local_size(tree);
    if(left <= k && k < left+own){
      assert(own == 1);
      removed = tree;
      Node* low = std::exchange(tree->lp,nullptr);
      Node* high = std::exchange(tree->rp,nullptr);
      return merge_nodes(low,high);
    }
    Node** branch = &tree->lp;
    if(k >= left+own){k -= left+own; branch = &tree->rp;}
    *branch = erase_node(*branch,k,removed);
    if(*branch) (*branch)->pp = tree;
    Ops::pull(tree);
    return tree;
  }
  template<class Predicate> std::pair<Node*,Node*> split_prefix_impl(Node* p, const Predicate& in_prefix){
    if(!p) return {nullptr,nullptr};
    Ops::push(p);
    p->pp = nullptr;
    if(in_prefix(p)){
      auto [left,right] = split_prefix_impl(p->rp,in_prefix);
      p->rp = left;
      if(left) left->pp = p;
      Ops::pull(p);
      return {p,right};
    }
    auto [left,right] = split_prefix_impl(p->lp,in_prefix);
    p->lp = right;
    if(right) right->pp = p;
    Ops::pull(p);
    return {left,p};
  }
  template<class Less> Node* meld_multi_impl(Node* a, Node* b, const Less& less){
    if(!a || !b) return a ? a : b;
    Node* top;
    if(Ops::priority(a) >= Ops::priority(b)){
      Ops::push(a);
      auto [left,right] = split_prefix_impl(b,[&](Node* p){return less(p,a);});
      a->lp = meld_multi_impl(a->lp,left,less);
      a->rp = meld_multi_impl(a->rp,right,less);
      top = a;
    }else{
      Ops::push(b);
      auto [left,right] = split_prefix_impl(a,[&](Node* p){return !less(b,p);});
      b->lp = meld_multi_impl(left,b->lp,less);
      b->rp = meld_multi_impl(right,b->rp,less);
      top = b;
    }
    if(top->lp) top->lp->pp = top;
    if(top->rp) top->rp->pp = top;
    Ops::pull(top);
    return top;
  }
public:
  explicit TreapBase(std::uint64_t seed = std::chrono::steady_clock::now().time_since_epoch().count()) : state(seed ? seed : 1){}
  void prepare(Node* p){state ^= state << 7; state ^= state >> 9; Ops::priority(p) = std::uint32_t(state >> 16);}
  Node* access_node(Node*&, Node* p){return p;}
  Node* access_pushed(Node*&, Node* p){return p;}
  Node* access(Node*& root, int k){return Links::locate(root,k).first;}
  Node* join(Node* l, Node* r){auto p = merge_nodes(l,r); if(p) p->pp = nullptr; return p;}
  void insert_at(Node*& root, int k, Node* node){
    assert(0 <= k && k <= Ops::size(root));
    root = insert_node(root,k,node);
    root->pp = nullptr;
  }
  Node* erase_at(Node*& root, int k){
    assert(0 <= k && k < Ops::size(root));
    Node* removed = nullptr;
    root = erase_node(root,k,removed);
    if(root) root->pp = nullptr;
    removed->pp = nullptr;
    return removed;
  }
  std::pair<Node*,Node*> split(Node* p, int k){
    assert(0 <= k && k <= Ops::size(p));
    if(k == 0) return {nullptr,p};
    if(k == Ops::size(p)) return {p,nullptr};
    auto [l,r] = split_nodes(p,k);
    l->pp = r->pp = nullptr;
    return {l,r};
  }
  template<class Cut> std::pair<Node*,Node*> split_weighted(Node* root, int k, Cut cut){
    if(k == 0) return {nullptr,root};
    if(k == Ops::size(root)) return {root,nullptr};
    auto [a,b,block,offset] = split_block(root,k);
    if(!block) return {a,b};
    auto [first,second] = cut(block,offset);
    Ops::pull(first);
    return {join(a,first),join(second,b)};
  }
  template<class Predicate> std::pair<Node*,Node*> split_prefix(Node* root, Predicate in_prefix){
    return split_prefix_impl(root,in_prefix);
  }
  // Equivalent elements in a precede those in b.
  template<class Less> Node* meld_multi(Node* a, Node* b, Less less){
    Node* root = meld_multi_impl(a,b,less);
    if(root) root->pp = nullptr;
    return root;
  }
  template<bool Reverse, class Local, class Combine>
  auto fold_range(Node* root, int l, int r, const Local& local, const Combine& combine) -> decltype(local(root,l,r)){
    while(true){
      if(l == 0 && r == Ops::size(root)) return Reverse ? root->rsum : root->lsum;
      Ops::push(root);
      int left = Ops::size(root->lp), end = left+Ops::local_size(root);
      if(r <= left){root = root->lp; continue;}
      if(l >= end){root = root->rp; l -= end; r -= end; continue;}
      auto result = local(root,std::max(l,left)-left,std::min(r,end)-left);
      if(l < left){
        auto prefix = fold_range<Reverse>(root->lp,l,left,local,combine);
        if constexpr(Reverse) result = combine(result,prefix);
        else result = combine(prefix,result);
      }
      if(r > end){
        auto suffix = fold_range<Reverse>(root->rp,0,r-end,local,combine);
        if constexpr(Reverse) result = combine(suffix,result);
        else result = combine(result,suffix);
      }
      return result;
    }
  }
  // Restore the heap after attaching a newly allocated block at an inorder gap.
  Node* bubble_up(Node* p){
    while(p->pp && Ops::priority(p) > Ops::priority(p->pp)) Links::rotate(p);
    Node* root = p;
    for(Node* q = p->pp; q; q = q->pp){Ops::pull(q); root = q;}
    return root;
  }
  template<class Factory> Node* build(int n, Factory at){
    return build_cartesian<Node*>(n,nullptr,at, [](Node* a, Node* b){return Ops::priority(a) > Ops::priority(b);}, [](Node* p, Node* l){p->lp = l; if(l) l->pp = p;}, [](Node* p, Node* r){p->rp = r; if(r) r->pp = p;}, [](Node* p){Ops::pull(p);}, [](Node* p){return p->pp;});
  }
};

}  // namespace soryuusi::internal
