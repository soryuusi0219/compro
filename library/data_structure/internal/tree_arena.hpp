// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/internal/tree_arena.hpp
#pragma once
#include <utility>
#include <vector>
#include "node_pool.hpp"

namespace soryuusi::internal {

template<class Ops, template<class,class> class Backend>
struct TreeArena {
  using Node = typename Ops::Node;
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
  void release(Node* p){free_nodes.push_back(p);}
};

}  // namespace soryuusi::internal
