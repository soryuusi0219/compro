// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/splay_tree.hpp
#pragma once
#include "library/data_structure/internal/reversible_monoid.hpp"
#include "library/data_structure/internal/sequence_base.hpp"
#include "library/data_structure/internal/splay_base.hpp"

namespace soryuusi {

template<class T, auto op, auto e>
using SplayTree = internal::SequenceBase<internal::ReversibleMonoid<T,op,e>,internal::SplayBase>;
template<class T, auto op, auto e>
using splay_tree = SplayTree<T,op,e>;

}  // namespace soryuusi
