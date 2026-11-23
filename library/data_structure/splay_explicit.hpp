// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/splay_explicit.hpp
#pragma once
#include <functional>
#include "internal/explicit_base.hpp"
#include "internal/splay_base.hpp"

namespace soryuusi {

template<class Key, class S, auto op, auto e, bool Multi = true, class Compare = std::less<Key>>
using SplayExplicit = internal::ExplicitArena<Key,internal::ExplicitMonoid<Key,S,op,e,internal::NoTreeAction,internal::EmptyTreeState>,internal::SplayBase,Multi,Compare>;
template<class Key, class S, auto op, auto e, class F, auto mapping, auto composition, auto id, bool Multi = true, class Compare = std::less<Key>>
using LazySplayExplicit = internal::ExplicitArena<Key,internal::ExplicitMonoid<Key,S,op,e,internal::TreeAction<F,mapping,composition,id>,internal::EmptyTreeState>,internal::SplayBase,Multi,Compare>;

template<class Key, class S, auto op, auto e, class F, auto key_mapping, auto composition, auto id, bool Multi = true, class Compare = std::less<Key>>
using MonotoneSplayExplicit = internal::ExplicitArena<Key, internal::ExplicitMonoid<Key,S,op,e,internal::NoTreeAction,internal::EmptyTreeState, internal::TreeAction<F,key_mapping,composition,id>>, internal::SplayBase,Multi,Compare>;

template<class Key, class S, auto op, auto e, class KF, auto key_mapping, auto key_composition, auto key_id, class F, auto mapping, auto composition, auto id, bool Multi = true, class Compare = std::less<Key>>
using LazyMonotoneSplayExplicit = internal::ExplicitArena<Key, internal::ExplicitMonoid<Key,S,op,e,internal::TreeAction<F,mapping,composition,id>, internal::EmptyTreeState,internal::TreeAction<KF,key_mapping,key_composition,key_id>>, internal::SplayBase,Multi,Compare>;

}  // namespace soryuusi
