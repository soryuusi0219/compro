// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/treap_sortable.hpp
#pragma once
#include <functional>
#include "internal/sortable_base.hpp"
#include "internal/treap_base.hpp"

namespace soryuusi {

template<class Key, class S, auto op, auto e,
         class Compare = std::less<Key>>
using TreapSortable = internal::SortableArena<Key,
    internal::ExplicitMonoid<Key,S,op,e,internal::NoTreeAction,
                             internal::SortableInnerState<internal::TreapPriority>>,
    internal::TreapBase,internal::TreapPriority,Compare>;

template<class Key, class S, auto op, auto e,
         class F, auto mapping, auto composition, auto id,
         class Compare = std::less<Key>>
using LazyTreapSortable = internal::SortableArena<Key,
    internal::ExplicitMonoid<Key,S,op,e,internal::TreeAction<F,mapping,composition,id>,
                             internal::SortableInnerState<internal::TreapPriority>>,
    internal::TreapBase,internal::TreapPriority,Compare>;

template<class Key, class S, auto op, auto e,
         class KF, auto key_mapping, auto key_composition, auto key_id,
         class Compare = std::less<Key>>
using MonotoneTreapSortable = internal::SortableArena<Key,
    internal::ExplicitMonoid<Key,S,op,e,internal::NoTreeAction,
                             internal::SortableInnerState<internal::TreapPriority>,
                             internal::TreeAction<KF,key_mapping,key_composition,key_id>>,
    internal::TreapBase,internal::TreapPriority,Compare>;

template<class Key, class S, auto op, auto e,
         class KF, auto key_mapping, auto key_composition, auto key_id,
         class F, auto mapping, auto composition, auto id,
         class Compare = std::less<Key>>
using LazyMonotoneTreapSortable = internal::SortableArena<Key,
    internal::ExplicitMonoid<Key,S,op,e,internal::TreeAction<F,mapping,composition,id>,
                             internal::SortableInnerState<internal::TreapPriority>,
                             internal::TreeAction<KF,key_mapping,key_composition,key_id>>,
    internal::TreapBase,internal::TreapPriority,Compare>;

}  // namespace soryuusi
