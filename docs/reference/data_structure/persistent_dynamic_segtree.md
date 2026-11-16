# persistent_dynamic_segtree

[索引](../README.md) / [実装](../../../library/data_structure/persistent_dynamic_segtree.hpp)

## 機能・前提条件

添字 `[0,n)` の永続一点代入・区間モノイド集約。`op` は結合的、`e()` は単位元。区間は半開区間で、境界探索の述語は単調かつ `f(e()) == true`。通常版のDynamic_Segtreeと同時includeできます。

`Root` は不透明な借用版ハンドルです。コピーは O(1) でノードを共有しますが、公開済みノードは変更しません。更新は新しい版を返し、検索は入力版を変更せず、ノードも確保しません。管理オブジェクトはコピー禁止・move可能で、版は移動先で引き続き使用できます。管理領域の破棄・move代入による置換後は、その領域の版を使用できません。別の管理領域の版を混ぜる操作は不可で、デバッグ時には所有元を検査します。移動元の管理オブジェクトは破棄または再代入してください。

## インターフェース

```cpp
#include "library/data_structure/persistent_dynamic_segtree.hpp"
template<class S, auto op, auto e> struct PersistentDynamicSegtree;
// 別名: Persistent_Dynamic_Segtree<S,op,e>
using Root = /* opaque handle */;
using NP = Root;
PersistentDynamicSegtree(int n);
Root empty_root() const;
Root instance() const; // 互換名
Root set(Root root, int i, S x);
S get(Root root, int i) const;
S prod(Root root, int l, int r) const;
S all_prod(Root root) const;
template<class F> int max_right(Root root, int l, F f) const;
template<class F> int min_left(Root root, int r, F f) const;
std::size_t allocated_nodes() const;
```

## 計算量

領域幅U。set/get/prod/境界探索は最悪 O(log U)、setは O(log U)ノード追加。全体集約・版コピーは O(1)。Q更新の空間は O(Q log U)。

## 依存関係

- [internal/node_pool.hpp](internal/node_pool.md)

## Verify・利用例

- [unit/pointer_structures](../../../verify/unit/pointer_structures.cpp)
- [library_checker/ordered_set_persistent_dynamic_segment_tree](../../../verify/library_checker/data_structure/ordered_set_persistent_dynamic_segment_tree.cpp)
