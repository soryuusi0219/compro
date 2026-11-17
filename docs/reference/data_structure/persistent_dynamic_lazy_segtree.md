# persistent_dynamic_lazy_segtree

[索引](../README.md) / [実装](../../../library/data_structure/persistent_dynamic_lazy_segtree.hpp)

## 機能・前提条件

添字 `[0,n)` の永続遅延セグメント木。半開区間の作用・集約に加え、他の版の同じ区間から値をコピーできます。`default_prod(l,r)` は未設定区間の集約（既定e）で区間分割とopに整合させます。`composition(f,g)` は f∘g。長さ依存の作用にはSに長さを含めてください。

`Root` は不透明な借用版ハンドルです。コピーは O(1) でノードを共有しますが、公開済みノードは変更しません。更新は新しい版を返し、検索は入力版を変更せず、ノードも確保しません。管理オブジェクトはコピー禁止・move可能で、版は移動先で引き続き使用できます。管理領域の破棄・move代入による置換後は、その領域の版を使用できません。別の管理領域の版を混ぜる操作は不可で、デバッグ時には所有元を検査します。移動元の管理オブジェクトは破棄または再代入してください。 区間コピーもコピー元の版を変更しません。

## インターフェース

```cpp
#include "library/data_structure/persistent_dynamic_lazy_segtree.hpp"
template<class S, auto op, auto e, class F, auto mapping, auto composition, auto id>
struct Persistent_Dynamic_LazySegtree;
using Root = /* opaque handle */;
Persistent_Dynamic_LazySegtree(int n);
template<class X> Persistent_Dynamic_LazySegtree(int n, X default_prod);
Root empty_root() const;
Root instance() const;
Root set(Root root, int i, S x);
Root apply(Root root, int l, int r, F f);
Root copy_range(Root destination, Root source, int l, int r);
Root copy_range_with_apply(Root destination, Root source, int l, int r, F f);
Root copy(Root source, Root destination, int l, int r); // 旧引数順
Root copy_with_apply(Root source, Root destination, int l, int r, F f);
S get(Root root, int i) const;
S prod(Root root, int l, int r) const;
S all_prod(Root root) const;
std::size_t allocated_nodes() const;
```

## 計算量

領域幅U。更新・区間コピー・取得・集約は最悪 O(log U)、更新・区間コピーは O(log U)ノード追加。全体集約・版コピーは O(1)。Q更新に O(Q log U)空間。検索はノードを追加しません。各作用・default_prodを O(1) とします。

## 依存関係

- [internal/node_pool.hpp](internal/node_pool.md)

## Verify・利用例

- [unit/pointer_structures](../../../verify/unit/pointer_structures.cpp)
- [library_checker/persistent_range_affine_range_sum](../../../verify/library_checker/data_structure/persistent_range_affine_range_sum.cpp)
