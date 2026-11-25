// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/cartesian_tree.hpp
#pragma once
#include <tuple>
#include <utility>
#include <vector>
#include "internal/cartesian_tree_base.hpp"

namespace soryuusi {

using namespace std;

template<class T>
tuple<vector<int>,vector<int>,vector<int>,int> cartesian_tree(const vector<T>& v, bool b = false){
  int n = v.size();
  vector<int> p(n,-1);
  vector<int> l(n,-1);
  vector<int> r(n,-1);
  int top = internal::build_cartesian<int>(n,-1,[](int i){return i;}, [&](int i, int j){return (v[i] > v[j]) == b;}, [&](int i, int j){l[i] = j; if(j >= 0) p[j] = i;}, [&](int i, int j){r[i] = j; if(j >= 0) p[j] = i;}, [](int){}, [&](int i){return p[i];});
  if(n == 0) top = 0; // Preserve the existing empty-input convention.
  return {std::move(p),std::move(l),std::move(r),top};
}

}  // namespace soryuusi
