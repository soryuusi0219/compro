// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/link_cut.hpp
#pragma once
#include "library/data_structure/internal/link_cut_base.hpp"
#include "library/data_structure/internal/reversible_monoid.hpp"

namespace soryuusi {

template<class T, auto op, auto e>
using linkcuttree = internal::LinkCutBase<internal::ReversibleMonoid<T,op,e,internal::NoTreeAction,false,true>>;

}  // namespace soryuusi
