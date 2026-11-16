// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/treap.hpp
#pragma once
#include "library/data_structure/internal/reversible_monoid.hpp"
#include "library/data_structure/internal/sequence_base.hpp"
#include "library/data_structure/internal/treap_base.hpp"

namespace soryuusi {

template<class T, auto op, auto e>
using TreapSequence = internal::SequenceArena<internal::ReversibleMonoid<T,op,e,internal::NoTreeAction,true,false,internal::TreapPriority>,internal::TreapBase>;
template<class T, auto op, auto e, class F, auto mapping, auto composition, auto id>
using LazyTreapSequence = internal::SequenceArena<internal::ReversibleMonoid<T,op,e,internal::TreeAction<F,mapping,composition,id>,true,false,internal::TreapPriority>,internal::TreapBase>;

}  // namespace soryuusi
