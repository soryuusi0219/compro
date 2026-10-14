// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/li_chao_tree_line.hpp
#pragma once

namespace soryuusi {

template<class T>
struct Line {
  T a, b;
  T get(T x) { return a * x + b; }
};

template<class T, T infinity>
Line<T> INF_Line() {
  return {0, infinity};
}

}  // namespace soryuusi
