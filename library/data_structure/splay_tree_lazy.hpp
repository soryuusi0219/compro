// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/splay_tree_lazy.hpp
#pragma once
#include "library/data_structure/internal/reversible_monoid.hpp"
#include "library/data_structure/internal/sequence_base.hpp"
#include "library/data_structure/internal/splay_base.hpp"

namespace soryuusi {

template<class T, auto op, auto e, class F, auto mapping, auto composition, auto id>
using LazySplayTree = internal::SequenceBase<internal::ReversibleMonoid<T,op,e,internal::TreeAction<F,mapping,composition,id>>,internal::SplayBase>;
template<class T, auto op, auto e, class F, auto mapping, auto composition, auto id>
using lazy_splay_tree = LazySplayTree<T,op,e,F,mapping,composition,id>;

}  // namespace soryuusi
