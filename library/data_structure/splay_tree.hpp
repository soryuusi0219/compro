// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/splay_tree.hpp
#pragma once
#include "internal/reversible_monoid.hpp"
#include "internal/sequence_base.hpp"
#include "internal/splay_base.hpp"

namespace soryuusi {

template<class T, auto op, auto e>
using SplayTree = internal::SequenceArena<internal::ReversibleMonoid<T,op,e>,internal::SplayBase>;
template<class T, auto op, auto e, class F, auto mapping, auto composition, auto id>
using LazySplayTree = internal::SequenceArena<internal::ReversibleMonoid<T,op,e,internal::TreeAction<F,mapping,composition,id>>,internal::SplayBase>;

}  // namespace soryuusi
