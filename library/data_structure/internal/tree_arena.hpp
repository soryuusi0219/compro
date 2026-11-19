// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/internal/tree_arena.hpp
#pragma once
#include <memory>
#include <unordered_map>
#include <utility>
#include <vector>
#include "node_pool.hpp"

namespace soryuusi::internal {

template<class Ops, template<class,class> class Backend>
struct TreeArena {
  using Node = typename Ops::Node;
private:
  struct Reference {
    std::weak_ptr<TreeArena> owner;
    Node* node;
  };
public:
  class NodeRef {
    friend struct TreeArena;
    std::shared_ptr<Reference> state;
    explicit NodeRef(std::shared_ptr<Reference> ref) : state(std::move(ref)){}
  public:
    NodeRef() = default;
  };
private:
  std::unordered_map<Node*,std::weak_ptr<Reference>> references;
  std::vector<Node*> path;
public:
  NodePool<Node> pool;
  std::vector<Node*> free_nodes;
  Backend<Node,Ops> engine;
  template<class... Args> Node* create(Args&&... args){
    Node* p;
    if(free_nodes.empty()) p = pool.create(std::forward<Args>(args)...);
    else{p = free_nodes.back(); free_nodes.pop_back(); *p = Node(std::forward<Args>(args)...);}
    engine.prepare(p);
    return p;
  }
  void release(Node* p){
    if(auto it = references.find(p); it != references.end()){
      if(auto ref = it->second.lock()) ref->node = nullptr;
      references.erase(it);
    }
    free_nodes.push_back(p);
  }
  NodeRef reference(const std::shared_ptr<TreeArena>& owner, Node* p){
    auto& slot = references[p];
    auto ref = slot.lock();
    if(!ref){ref = std::make_shared<Reference>(Reference{owner,p}); slot = ref;}
    return NodeRef(std::move(ref));
  }
  void transfer_reference(Node* old, const std::shared_ptr<TreeArena>& destination, Node* replacement){
    if(auto it = references.find(old); it != references.end()){
      if(auto ref = it->second.lock()){
        destination->references[replacement] = ref;
        ref->owner = destination;
        ref->node = replacement;
      }
      references.erase(it);
    }
  }
  Node* node_of(const NodeRef& ref) const {
    if(!ref.state || ref.state->owner.lock().get() != this) return nullptr;
    return ref.state->node;
  }
  int index_of_node(Node*& root, Node* p){
    if(!root || !p) return -1;
    path.clear();
    for(Node* q = p; q; q = q->pp) path.push_back(q);
    if(path.back() != root) return -1;
    for(auto it = path.rbegin(); it != path.rend(); ++it) Ops::push(*it);
    int index = Ops::size(p->lp);
    for(Node* q = p; q->pp; q = q->pp)
      if(q == q->pp->rp) index += Ops::local_size(q->pp)+Ops::size(q->pp->lp);
    if constexpr(requires {engine.access_node(root,p);}) engine.access_node(root,p);
    else engine.access(root,index);
    return index;
  }
  int index_of(Node*& root, const NodeRef& ref){return index_of_node(root,node_of(ref));}
  template<class Function> static void visit_in_order(Node* p, Function visit, bool reverse = false){
    std::vector<Node*> stack;
    while(p || !stack.empty()){
      while(p){Ops::push(p); stack.push_back(p); p = reverse ? p->rp : p->lp;}
      p = stack.back(); stack.pop_back();
      visit(p); p = reverse ? p->lp : p->rp;
    }
  }
};

}  // namespace soryuusi::internal
