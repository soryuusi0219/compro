// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/internal/splay_base.hpp
#pragma once
#include <cassert>
#include <utility>
#include <vector>
#include "binary_tree_ops.hpp"

namespace soryuusi::internal {

template<class Node, class Ops>
class SplayBase {
  using Links = BinaryTreeOps<Node,Ops>;
  std::vector<Node*> ancestors;
  static void rotate_path(Node* p, Node* stop){
    while(p->pp != stop){
      Node* q = p->pp;
      Node* r = q->pp;
      if(r != stop){
        // The next rotation pulls the top node again.
        if((q->lp == p) == (r->lp == q)) Links::template rotate<false>(q);
        else Links::template rotate<false>(p);
      }
      Links::rotate(p);
    }
  }
public:
  void prepare(Node*){}
  void splay(Node* p){
    if(!p) return;
    ancestors.clear();
    for(Node* q = p;;q = q->pp){ancestors.push_back(q); if(Links::is_root(q)) break;}
    for(auto it = ancestors.rbegin(); it != ancestors.rend(); ++it) Ops::push(*it);
    rotate_path(p,ancestors.back()->pp);
  }
  Node* access_node(Node*& root, Node* p){splay(p); root = p; return p;}
  Node* access_pushed(Node*& root, Node* p){
    rotate_path(p,nullptr);
    root = p;
    return p;
  }
  Node* access(Node*& root, int k){
    auto [p,offset] = Links::locate(root,k);
    assert(offset == 0); // Sequence interface: one value per node.
    (void)offset;
    return access_pushed(root,p);
  }
  // Split at the end of a prefix described by an inorder-monotone predicate.
  template<class Predicate> std::pair<Node*,Node*> split_prefix(Node* root, Predicate in_prefix){
    Node* last = nullptr;
    bool before = false;
    for(Node* p = root; p; p = before ? p->rp : p->lp){
      Ops::push(p);
      last = p;
      before = in_prefix(p);
    }
    if(!last) return {nullptr,nullptr};
    access_pushed(root,last);
    Node*& cut = before ? last->rp : last->lp;
    Node* other = cut;
    if(other){
      cut = nullptr;
      other->pp = nullptr;
      Ops::pull(last);
    }
    return before ? std::pair{last,other} : std::pair{other,last};
  }
  // The positions here are between nodes, not inside weighted blocks.
  template<bool Update = true, class Function> auto with_range(Node*& root, int l, int r, Function function){
    int n = Ops::size(root);
    Node* before = nullptr;
    Node* after = nullptr;
    Node** middle = &root;
    if(l){
      before = access(root,l-1);
      middle = &before->rp;
    }
    if(r < n){
      after = Links::locate(*middle,r-l).first;
      rotate_path(after,before);
      *middle = after;
      middle = &after->lp;
    }
    auto result = function(*middle);
    if constexpr(Update){
      if(*middle) (*middle)->pp = after ? after : before;
      if(after) Ops::pull(after);
      if(before) Ops::pull(before);
    }
    return result;
  }
  // Read weighted blocks in place, including partial first and last blocks.
  template<bool Reverse, class Local, class Combine>
  auto fold_range(Node*& root, int l, int r, Local local, Combine combine) -> decltype(local(root,l,r)){
    if(l == 0 && r == Ops::size(root)) return Reverse ? root->rsum : root->lsum;
    auto [first,offset] = Links::locate(root,l);
    access_pushed(root,first);
    int length = Ops::local_size(first);
    int right_count = r-l-(length-offset);
    if(right_count <= 0) return local(first,offset,offset+r-l);
    auto [last,end] = Links::locate(first->rp,right_count-1);
    rotate_path(last,first);
    auto result = local(first,offset,length);
    auto append = [&](const auto& value){
      if constexpr(Reverse) result = combine(value,result);
      else result = combine(result,value);
    };
    if(last->lp) append(Reverse ? last->lp->rsum : last->lp->lsum);
    append(local(last,0,end+1));
    return result;
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
  void insert_at(Node*& root, int k, Node* node){
    auto [left,right] = split(root,k);
    node->lp = left;
    node->rp = right;
    if(left) left->pp = node;
    if(right) right->pp = node;
    Ops::pull(node);
    root = node;
  }
  Node* erase_at(Node*& root, int k){
    Node* node = access(root,k);
    Node* left = std::exchange(node->lp,nullptr);
    Node* right = std::exchange(node->rp,nullptr);
    if(left) left->pp = nullptr;
    if(right) right->pp = nullptr;
    root = join(left,right);
    return node;
  }
  template<class Cut> std::pair<Node*,Node*> split_weighted(Node* root, int k, Cut cut){
    if(k == 0) return {nullptr,root};
    if(k == Ops::size(root)) return {root,nullptr};
    auto [block,offset] = Links::locate(root,k);
    access_pushed(root,block);
    Node* left = std::exchange(block->lp,nullptr);
    if(left) left->pp = nullptr;
    if(offset == 0){
      Ops::pull(block);
      return {left,block};
    }
    Node* right = std::exchange(block->rp,nullptr);
    auto [first,second] = cut(block,offset);
    first->lp = left;
    if(left) left->pp = first;
    second->rp = right;
    if(right) right->pp = second;
    Ops::pull(first);
    Ops::pull(second);
    return {first,second};
  }
  Node* join(Node* left, Node* right){
    if(!left || !right) return left ? left : right;
    Node* p = left;
    Ops::push(p);
    while(p->rp){p = p->rp; Ops::push(p);}
    access_pushed(left,p);
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
