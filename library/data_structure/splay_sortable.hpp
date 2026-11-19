// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/splay_sortable.hpp
#pragma once
#include <functional>
#include "internal/sortable_base.hpp"
#include "internal/splay_base.hpp"

namespace soryuusi {

template<class Key, class S, auto op, auto e,
         class Compare = std::less<Key>>
using SplaySortable = internal::SortableArena<Key,
    internal::ExplicitMonoid<Key,S,op,e,internal::NoTreeAction,
                             internal::SortableInnerState<internal::EmptyTreeState>>,
    internal::SplayBase,internal::EmptyTreeState,Compare>;

template<class Key, class S, auto op, auto e,
         class F, auto mapping, auto composition, auto id,
         class Compare = std::less<Key>>
using LazySplaySortable = internal::SortableArena<Key,
    internal::ExplicitMonoid<Key,S,op,e,internal::TreeAction<F,mapping,composition,id>,
                             internal::SortableInnerState<internal::EmptyTreeState>>,
    internal::SplayBase,internal::EmptyTreeState,Compare>;

template<class Key, class S, auto op, auto e,
         class KF, auto key_mapping, auto key_composition, auto key_id,
         class Compare = std::less<Key>>
using MonotoneSplaySortable = internal::SortableArena<Key,
    internal::ExplicitMonoid<Key,S,op,e,internal::NoTreeAction,
                             internal::SortableInnerState<internal::EmptyTreeState>,
                             internal::TreeAction<KF,key_mapping,key_composition,key_id>>,
    internal::SplayBase,internal::EmptyTreeState,Compare>;

template<class Key, class S, auto op, auto e,
         class KF, auto key_mapping, auto key_composition, auto key_id,
         class F, auto mapping, auto composition, auto id,
         class Compare = std::less<Key>>
using LazyMonotoneSplaySortable = internal::SortableArena<Key,
    internal::ExplicitMonoid<Key,S,op,e,internal::TreeAction<F,mapping,composition,id>,
                             internal::SortableInnerState<internal::EmptyTreeState>,
                             internal::TreeAction<KF,key_mapping,key_composition,key_id>>,
    internal::SplayBase,internal::EmptyTreeState,Compare>;

}  // namespace soryuusi
