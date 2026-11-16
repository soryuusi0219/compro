// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/splay_explicit.hpp
#pragma once
#include <functional>
#include "library/data_structure/internal/explicit_base.hpp"
#include "library/data_structure/internal/splay_base.hpp"

namespace soryuusi {

template<class Key, class S, auto op, auto e, bool Multi = true, class Compare = std::less<Key>>
using SplayExplicit = internal::ExplicitArena<Key,internal::ExplicitMonoid<Key,S,op,e,internal::NoTreeAction,internal::EmptyTreeState>,internal::SplayBase,Multi,Compare>;
template<class Key, class S, auto op, auto e, class F, auto mapping, auto composition, auto id, bool Multi = true, class Compare = std::less<Key>>
using LazySplayExplicit = internal::ExplicitArena<Key,internal::ExplicitMonoid<Key,S,op,e,internal::TreeAction<F,mapping,composition,id>,internal::EmptyTreeState>,internal::SplayBase,Multi,Compare>;

}  // namespace soryuusi
