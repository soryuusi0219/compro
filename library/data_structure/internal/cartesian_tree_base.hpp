// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/internal/cartesian_tree_base.hpp
#pragma once
#include <vector>

namespace soryuusi::internal {

// at(i) yields an inorder handle. Setters also maintain parent links, if any.
// finish is called exactly once, after both children have been completed.
template<class Handle, class Factory, class Higher, class Left, class Right, class Finish>
Handle build_cartesian(int n, Handle nil, Factory at, Higher higher, Left left, Right right, Finish finish){
  std::vector<Handle> stack;
  stack.reserve(n);
  for(int i = 0; i < n; ++i){
    Handle p = at(i), last = nil;
    while(!stack.empty() && higher(p,stack.back())){
      last = stack.back(); stack.pop_back();
      finish(last);
    }
    left(p,last);
    right(p,nil);
    if(!stack.empty()) right(stack.back(),p);
    stack.push_back(p);
  }
  Handle root = stack.empty() ? nil : stack.front();
  while(!stack.empty()){finish(stack.back()); stack.pop_back();}
  return root;
}

}  // namespace soryuusi::internal
