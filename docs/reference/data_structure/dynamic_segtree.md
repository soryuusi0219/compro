# dynamic_segtree

[索引](../README.md) / [実装](../../../library/data_structure/dynamic_segtree.hpp)

## 機能・前提条件

添字 `[0,n)` の一点代入・モノイド集約。`op` は結合的、`e()` は単位元で未設定値です。区間は半開区間です。境界探索の述語は単調で `f(e()) == true` を要求します。

コピー禁止、move可能。移動元の木は空になり、再利用できます。`clone()` はノードを共有しない独立複製です。ノードはオブジェクトのプールが所有し、個別に解放しません。

## インターフェース

```cpp
#include "library/data_structure/dynamic_segtree.hpp"
template<class S, auto op, auto e> struct Dynamic_Segtree;
Dynamic_Segtree(int n);
void set(int i, S x);
S get(int i);
S prod(int l, int r);
S all_prod();
template<class F> int max_right(int l, F f);
template<class F> int min_left(int r, F f);
Dynamic_Segtree clone() const;
std::size_t allocated_nodes() const;
```

## 計算量

領域幅 U=n、更新数 Q、確保済みノード数 K とします。更新・取得・集約・境界探索は最悪 O(log U)、全体集約は O(1)、空間は O(K)（K=O(Q log U)）。cloneは O(K) 時間・追加空間です。move構築は O(1)、move代入は置換前領域の破棄時間を要します。

## 依存関係

- [internal/node_pool.hpp](internal/node_pool.md)

## Verify・利用例

- [unit/pointer_structures](../../../verify/unit/pointer_structures.cpp)
- [library_checker/ordered_set_dynamic_segment_tree](../../../verify/library_checker/data_structure/ordered_set_dynamic_segment_tree.cpp)
