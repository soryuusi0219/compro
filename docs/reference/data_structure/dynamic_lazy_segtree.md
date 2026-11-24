# dynamic_lazy_segtree

[索引](../README.md) / [実装](../../../library/data_structure/dynamic_lazy_segtree.hpp)

## 機能・前提条件

巨大な添字領域 `[0,n)` の一点代入・区間作用・集約。`default_prod(l,r)` は未設定区間の集約で、省略時は `e()` です。空区間では単位元を返し、区間の分割と `op` に整合させてください。長さ依存の作用では S に長さを含めます。`composition(f,g)` は f∘g、mappingはモノイド集約と適合する必要があります。

空区間の `apply` はノードを作らず、空区間の `prod` は `e()` を返します。作用型に恒等作用との比較がない場合は内部の保留フラグで伝播を管理します。

コピー禁止、move可能。移動元の木は空になり、再利用できます。`clone()` はノードを共有しない独立複製です。ノードはオブジェクトのプールが所有し、個別に解放しません。 読み取りはconstで、遅延タグを伝播させず、ノードを確保しません。

## インターフェース

```cpp
#include "library/data_structure/dynamic_lazy_segtree.hpp"
template<class S, auto op, auto e, class F, auto mapping, auto composition, auto id>
struct Dynamic_LazySegtree;
Dynamic_LazySegtree(int n);
template<class X> Dynamic_LazySegtree(int n, X default_prod);
void set(int i, S x);
void apply(int l, int r, F f);
S get(int i) const;
S get(int i, S ignored) const; // 旧APIの互換ラッパー
S prod(int l, int r) const;
S all_prod();
Dynamic_LazySegtree clone() const;
std::size_t allocated_nodes() const;
```

## 計算量

領域幅 U=n、更新数 Q、確保済みノード数 K。更新・取得・集約は最悪 O(log U)、全体集約は O(1)。空間 O(K)、K=O(Q log U)。検索だけではKは増えません。cloneは O(K) 時間・追加空間。default_prod・各モノイド演算を O(1) とします。

## 依存関係

- [internal/node_pool.hpp](internal/node_pool.md)

## Verify・利用例

- [`unit/sparse_structures`](../../../verify/unit/sparse_structures.cpp)
- [unit/pointer_structures](../../../verify/unit/pointer_structures.cpp)
- [library_checker/range_affine_range_sum_large_array](../../../verify/library_checker/data_structure/range_affine_range_sum_large_array.cpp)
