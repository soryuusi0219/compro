// https://github.com/soryuusi0219/compro/blob/main/library/graph/undirected_cut_edge.hpp
#pragma once

namespace soryuusi {

template <class Cap = long long> struct UndirectedCutEdge {
  int from, to;
  Cap capacity;
};

}  // namespace soryuusi
