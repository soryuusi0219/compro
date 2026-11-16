// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/lazy_link_cut.hpp
#pragma once
#include "library/data_structure/internal/link_cut_base.hpp"
#include "library/data_structure/internal/reversible_monoid.hpp"

namespace soryuusi {

template<class T, auto op, auto e, class F, auto mapping, auto composition, auto id>
using lazy_linkcuttree = internal::LinkCutBase<internal::ReversibleMonoid<T,op,e,internal::TreeAction<F,mapping,composition,id>,false,true>>;

}  // namespace soryuusi
