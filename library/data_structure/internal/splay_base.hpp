// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/internal/splay_base.hpp
#pragma once
#include <cassert>
#include <utility>
#include <vector>
#include "library/data_structure/internal/binary_tree_ops.hpp"

namespace soryuusi::internal {

template<class Node, class Ops>
class SplayBase {
  using Links = BinaryTreeOps<Node,Ops>;
  std::vector<Node*> ancestors;
public:
  void prepare(Node*){}
  void splay(Node* p){
    if(!p) return;
    ancestors.clear();
    for(Node* q = p;;q = q->pp){ancestors.push_back(q); if(Links::is_root(q)) break;}
    for(auto it = ancestors.rbegin(); it != ancestors.rend(); ++it) Ops::push(*it);
    while(!Links::is_root(p)){
      Node* q = p->pp;
      if(!Links::is_root(q)){
        Node* r = q->pp;
        if((q->lp == p) == (r->lp == q)) Links::rotate(q);
        else Links::rotate(p);
      }
      Links::rotate(p);
    }
  }
  Node* access(Node*& root, int k){
    auto [p,offset] = Links::locate(root,k);
    assert(offset == 0); // Sequence interface: one value per node.
    (void)offset;
    splay(p);
    root = p;
    return p;
  }
  std::pair<Node*,Node*> split(Node* root, int k){
    assert(0 <= k && k <= Ops::size(root));
    if(k == 0) return {nullptr,root};
    if(k == Ops::size(root)) return {root,nullptr};
    Node* right = access(root,k);
    Node* left = right->lp;
    right->lp = nullptr;
    if(left) left->pp = nullptr;
    Ops::pull(right);
    return {left,right};
  }
  Node* join(Node* left, Node* right){
    if(!left || !right) return left ? left : right;
    Node* p = left;
    Ops::push(p);
    while(p->rp){p = p->rp; Ops::push(p);}
    splay(p);
    p->rp = right; right->pp = p;
    Ops::pull(p);
    return p;
  }
  template<class Factory> Node* build(int n, Factory at){
    auto dfs = [&](auto&& self, int l, int r) -> Node* {
      if(l == r) return nullptr;
      int m = l+(r-l)/2;
      Node* p = at(m);
      p->lp = self(self,l,m); p->rp = self(self,m+1,r);
      if(p->lp) p->lp->pp = p;
      if(p->rp) p->rp->pp = p;
      Ops::pull(p);
      return p;
    };
    return dfs(dfs,0,n);
  }
};

}  // namespace soryuusi::internal
