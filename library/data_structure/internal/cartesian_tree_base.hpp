// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/internal/cartesian_tree_base.hpp
#pragma once
#include <cstddef>
#include <type_traits>
#include <vector>

namespace soryuusi::internal {

// at(i) yields an inorder handle. With a parent accessor, at(i) starts with
// a nil parent, and the setters maintain parent links instead of using a stack.
template<class Handle, class Factory, class Higher, class Left, class Right, class Finish, class Parent = std::nullptr_t>
Handle build_cartesian(int n, Handle nil, Factory at, Higher higher, Left left, Right right, Finish finish, Parent parent = nullptr){
  constexpr bool use_stack = std::is_same_v<Parent,std::nullptr_t>;
  std::vector<Handle> stack;
  if constexpr(use_stack) stack.reserve(n);
  Handle top = nil, root = nil;
  auto has_top = [&]{
    if constexpr(use_stack) return !stack.empty();
    else return top != nil;
  };
  auto pop = [&]{
    Handle p = top;
    if constexpr(use_stack){
      stack.pop_back();
      top = stack.empty() ? nil : stack.back();
    } else top = parent(top);
    return p;
  };
  for(int i = 0; i < n; ++i){
    Handle p = at(i), last = nil;
    while(has_top() && higher(p,top)){
      last = pop();
      finish(last);
    }
    left(p,last);
    right(p,nil);
    if(has_top()) right(top,p);
    else root = p;
    if constexpr(use_stack) stack.push_back(p);
    top = p;
  }
  while(has_top()) finish(pop());
  return root;
}

}  // namespace soryuusi::internal
