// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/treap_explicit.hpp
#pragma once
#include <functional>
#include "library/data_structure/internal/explicit_base.hpp"
#include "library/data_structure/internal/treap_base.hpp"

namespace soryuusi {

template<class Key, class S, auto op, auto e, bool Multi = true, class Compare = std::less<Key>>
using TreapExplicit = internal::ExplicitArena<Key,internal::ExplicitMonoid<Key,S,op,e,internal::NoTreeAction,internal::TreapPriority>,internal::TreapBase,Multi,Compare>;
template<class Key, class S, auto op, auto e, class F, auto mapping, auto composition, auto id, bool Multi = true, class Compare = std::less<Key>>
using LazyTreapExplicit = internal::ExplicitArena<Key,internal::ExplicitMonoid<Key,S,op,e,internal::TreeAction<F,mapping,composition,id>,internal::TreapPriority>,internal::TreapBase,Multi,Compare>;

}  // namespace soryuusi
