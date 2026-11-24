// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/internal/link_cut_base.hpp
#pragma once
#include <cassert>
#include <iostream>
#include <utility>
#include <type_traits>
#include <vector>
#include "splay_base.hpp"

namespace soryuusi::internal {

// The auxiliary engine must support splay while preserving path-parent links.
template<class Ops, template<class,class> class Auxiliary = SplayBase>
class LinkCutBase {
  using Node = typename Ops::Node;
  using T = typename Ops::value_type;
  std::vector<Node> tree;
  Auxiliary<Node,Ops> engine;
  void check(int i) const {assert(0 <= i && i < size()); (void)i;}
  Node* access(Node* p){
    Node* last = nullptr;
    for(Node* q = p; q; q = q->pp){
      engine.splay(q);
      // last already points to q as its path-parent, and the old right child keeps q.
      if(q->rp != last){
        q->rp = last;
        Ops::pull(q);
      }
      last = q;
    }
    if(last != p) engine.splay(p);
    return last;
  }
  void make_root(Node* p){access(p); if(p->lp) Ops::toggle(p);}
public:
  explicit LinkCutBase(int n) : LinkCutBase(n,Ops::identity()){}
  LinkCutBase(int n, T initial){
    assert(n >= 0);
    tree.reserve(n);
    for(int i = 0; i < n; ++i){tree.emplace_back(initial); tree.back().label.index = i;}
  }
  LinkCutBase(const LinkCutBase&) = delete;
  LinkCutBase& operator=(const LinkCutBase&) = delete;
  LinkCutBase(LinkCutBase&& other) noexcept(std::is_nothrow_move_constructible_v<Auxiliary<Node,Ops>>) : tree(std::move(other.tree)), engine(std::move(other.engine)){other.tree.clear();}
  LinkCutBase& operator=(LinkCutBase&& other) noexcept(std::is_nothrow_move_assignable_v<Auxiliary<Node,Ops>>) {
    if(this != &other){tree = std::move(other.tree); engine = std::move(other.engine); other.tree.clear();}
    return *this;
  }
  int size() const {return tree.size();}
  [[nodiscard]] LinkCutBase clone() const {
    LinkCutBase result(0);
    result.tree = tree;
    for(auto& p : result.tree){
      if(p.pp) p.pp = &result.tree[p.pp->label.index];
      if(p.lp) p.lp = &result.tree[p.lp->label.index];
      if(p.rp) p.rp = &result.tree[p.rp->label.index];
    }
    return result;
  }
  int find_root(int i){
    check(i);
    Node* root = &tree[i];
    access(root);
    Node* p = root;
    while(p->lp){p = p->lp; Ops::push(p);}
    if(p != root){
      if constexpr(requires {engine.access_pushed(root,p);}) engine.access_pushed(root,p);
      else engine.splay(p);
    }
    return p->label.index;
  }
  bool connected(int i, int j){return find_root(i) == find_root(j);}
  bool same(int i, int j){return connected(i,j);}
  void link(int i, int j){
    check(i); check(j); assert(i != j && !connected(i,j));
    make_root(&tree[j]);
    tree[j].pp = &tree[i]; // The new edge becomes preferred on its first access.
  }
  void cut(int i, int j){
    check(i); check(j); assert(i != j);
    make_root(&tree[i]); access(&tree[j]);
    assert(tree[j].lp == &tree[i] && !tree[i].rp);
    tree[j].lp = nullptr; tree[i].pp = nullptr; Ops::pull(&tree[j]);
  }
  void expose(int i){check(i); access(&tree[i]);}
  void evert(int i){check(i); make_root(&tree[i]);}
  void set(int i, T value){check(i); engine.splay(&tree[i]); tree[i].value = value; Ops::pull(&tree[i]);}
  T get(int i){check(i); engine.splay(&tree[i]); return tree[i].value;}
  T prod(int i){check(i); access(&tree[i]); return tree[i].rsum;}
  T prod(int i, int j){check(j); make_root(&tree[j]); return prod(i);}
  void apply(int i, typename Ops::action_type f) requires Ops::has_action {check(i); access(&tree[i]); Ops::apply(&tree[i],f);}
  void apply(int i, int j, typename Ops::action_type f) requires Ops::has_action {check(j); make_root(&tree[j]); apply(i,f);}
  int lca(int i, int j){check(i); check(j); access(&tree[i]); return access(&tree[j])->label.index;}
  std::vector<int> get_path(int i, int j){
    check(i); check(j); make_root(&tree[j]); access(&tree[i]);
    std::vector<int> result;
    std::vector<Node*> stack;
    Node* p = &tree[i];
    while(p || !stack.empty()){
      while(p){Ops::push(p); stack.push_back(p); p = p->rp;}
      p = stack.back(); stack.pop_back(); result.push_back(p->label.index); p = p->lp;
    }
    return result;
  }
  void print(){
    for(int i = 0; i < size(); ++i){
      std::cout << "i:" << i << " p:";
      if(tree[i].pp) std::cout << tree[i].pp->label.index;
      std::cout << " l:"; if(tree[i].lp) std::cout << tree[i].lp->label.index;
      std::cout << " r:"; if(tree[i].rp) std::cout << tree[i].rp->label.index;
      std::cout << " rev:" << tree[i].rev << '\n';
    }
  }
};

}  // namespace soryuusi::internal
